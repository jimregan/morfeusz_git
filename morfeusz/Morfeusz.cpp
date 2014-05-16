/* 
 * File:   Morfeusz.cpp
 * Author: mlenart
 * 
 * Created on November 13, 2013, 5:21 PM
 */

#include <string>
#include <iostream>
#include "fsa/fsa.hpp"
#include "utils.hpp"
#include "data/default_fsa.hpp"
#include "Morfeusz.hpp"
#include "decoder/InterpretedChunksDecoder.hpp"
#include "charset/CharsetConverter.hpp"
#include "charset/charset_utils.hpp"
#include "charset/CaseConverter.hpp"
#include "segrules/segrules.hpp"
#include "const.hpp"
#include "deserializationUtils.hpp"
#include "charset/utf8.h"
#include "compressionByteUtils.hpp"

// TODO - konstruktor kopiujący działający Tak-Jak-Trzeba

using namespace std;

static MorfeuszOptions createDefaultOptions() {
    MorfeuszOptions res;
    res.caseSensitive = true;
    res.encoding = UTF8;
    res.debug = false;
    return res;
}

static void doShiftOrth(InterpretedChunk& from, InterpretedChunk& to) {
    to.prefixChunks.push_back(from);
    to.textStartPtr = from.textStartPtr;
    from.orthWasShifted = true;
}

static string debugInterpsGroup(unsigned char type, const char* startPtr, const char* endPtr) {
    stringstream res;
    res << "(" << (int) type << ", " << string(startPtr, endPtr) << "), ";
    return res.str();
}

static string debugAccum(vector<InterpretedChunk>& accum) {
    stringstream res;
    for (unsigned int i = 0; i < accum.size(); i++) {
        res << debugInterpsGroup(accum[i].segmentType, accum[i].textStartPtr, accum[i].textEndPtr);
        //        res << "(" << (int) accum[i].interpsGroup.type << ", " << string(accum[i].chunkStartPtr, accum[i].chunkStartPtr) << "), ";
    }
    return res.str();
}

static void feedStateDirectly(
        StateType& state,
        const char* inputStart,
        const char* inputEnd) {
    const char* currInput = inputStart;
    while (currInput != inputEnd && !state.isSink()) {
        state.proceedToNext(*currInput++);
    }
}

static void feedState(
        StateType& state,
        int codepoint) {
    std::string chars;
    UTF8CharsetConverter::getInstance().append(codepoint, chars);
    for (unsigned int i = 0; i < chars.length() && !state.isSink(); i++) {
        state.proceedToNext(chars[i]);
    }
}

Morfeusz::Morfeusz()
: analyzerEnv(DEFAULT_MORFEUSZ_CHARSET, ANALYZER, DEFAULT_FSA),
generatorEnv(DEFAULT_MORFEUSZ_CHARSET, GENERATOR, DEFAULT_SYNTH_FSA),
options(createDefaultOptions()),
accum(),
graph() {
    analyzerEnv.setCaseSensitive(options.caseSensitive);
    generatorEnv.setCaseSensitive(false);
}

inline const unsigned char* getInterpretationsPtr(const Environment& env, const InterpsGroup& ig) {
    if (env.getProcessorType() == ANALYZER) {
        if (hasCompressedOrthCasePatterns(*ig.ptr)) {
            return ig.ptr + 1;
        }
        else {
            const unsigned char* currPtr = ig.ptr + 1;
            unsigned char casePatternsNum = readInt8(currPtr);
            for (unsigned int i = 0; i < casePatternsNum; i++) {
                env.getCasePatternHelper().deserializeOneCasePattern(currPtr);
            }
            return currPtr;
        }
    }
    else {
        return ig.ptr;
    }
}

void Morfeusz::setAnalyzerFile(const string& filename) {
    this->analyzerEnv.setFSAFile(filename);
}

void Morfeusz::setGeneratorFile(const string& filename) {
    this->generatorEnv.setFSAFile(filename);
}

Morfeusz::~Morfeusz() {
}

void Morfeusz::processOneWord(
        const Environment& env,
        const char*& inputStart,
        const char* inputEnd,
        int startNodeNum,
        std::vector<MorphInterpretation>& results,
        bool insideIgnHandler) const {
    while (inputStart != inputEnd
            && isWhitespace(env.getCharsetConverter().peek(inputStart, inputEnd))) {
        env.getCharsetConverter().next(inputStart, inputEnd);
    }

    accum.clear();
    graph.clear();

    const char* currInput = inputStart;
    const SegrulesFSA& segrulesFSA = env.getCurrentSegrulesFSA();

    doProcessOneWord(env, currInput, inputEnd, segrulesFSA.initialState);

    if (!graph.empty()) {
        const InterpretedChunksDecoder& interpretedChunksDecoder = env.getInterpretedChunksDecoder();
        int srcNode = startNodeNum;
        const std::vector< std::vector<InflexionGraph::Edge> >& theGraph = graph.getTheGraph();
        for (unsigned int i = 0; i < theGraph.size(); i++) {
            const vector<InflexionGraph::Edge>& edges = theGraph[i];
            for (unsigned int j = 0; j < edges.size(); j++) {
                const InflexionGraph::Edge& e = edges[j];
                unsigned int targetNode = startNodeNum + e.nextNode;
                interpretedChunksDecoder.decode(srcNode, targetNode, e.chunk, results);
            }
            srcNode++;
        }
    }
    else if (inputStart != inputEnd
            && env.getProcessorType() == ANALYZER
            && !insideIgnHandler) {
        this->handleIgnChunk(env, inputStart, currInput, startNodeNum, results);
    }
    else if (inputStart != inputEnd) {
        this->appendIgnotiumToResults(env, string(inputStart, currInput), startNodeNum, results);
    }
    inputStart = currInput;
}

void Morfeusz::doProcessOneWord(
        const Environment& env,
        const char*& inputData,
        const char* inputEnd,
        const SegrulesState& segrulesState) const {
    if (this->options.debug) {
        cerr << "----------" << endl;
        cerr << "PROCESS: '" << inputData << "', already recognized: " << debugAccum(accum) << endl;
    }
    //    cerr << "doAnalyzeOneWord " << inputData << endl;
    const char* inputStart = inputData;
    const char* prevInput = inputData;
    const char* currInput = inputData;
    uint32_t codepoint = inputData == inputEnd ? 0 : env.getCharsetConverter().next(currInput, inputEnd);
    bool currCodepointIsWhitespace = isWhitespace(codepoint);
    vector<SegrulesState> newSegrulesStates;
    int codepointsNum = 0;

    StateType state = env.getFSA().getInitialState();

    while (!currCodepointIsWhitespace) {
        uint32_t normalizedCodepoint = env.getProcessorType() == ANALYZER
                ? env.getCaseConverter().toLower(codepoint)
                : codepoint;
        if (codepoint == normalizedCodepoint && &env.getCharsetConverter() == &UTF8CharsetConverter::getInstance()) {
            feedStateDirectly(state, prevInput, currInput);
        }
        else {
            feedState(state, normalizedCodepoint);
        }
        codepointsNum++;

        codepoint = currInput == inputEnd ? 0 : env.getCharsetConverter().peek(currInput, inputEnd);
        currCodepointIsWhitespace = isWhitespace(codepoint);
        string homonymId;
        if (env.getProcessorType() == GENERATOR && codepoint == 0x3A && currInput + 1 != inputEnd) {
            homonymId = string(currInput + 1, inputEnd);
            prevInput = currInput;
            currInput = inputEnd;
            codepoint = 0x00;
            currCodepointIsWhitespace = true;
        }
        if (state.isAccepting()) {
            InterpsGroupsReader& reader = const_cast<InterpsGroupsReader&>(state.getValue());
//            for (unsigned int i = 0; i < state.getValue().size(); i++) {
            while (reader.hasNext()) {
//                const InterpsGroup& ig = state.getValue()[i];
                const InterpsGroup& ig = reader.getNext();
                if (this->options.debug) {
                    cerr << "recognized: " << debugInterpsGroup(ig.type, inputStart, currInput) << " at: '" << inputStart << "'" << endl;
                }
                newSegrulesStates.clear();
                env.getCurrentSegrulesFSA().proceedToNext(ig.type, segrulesState, currCodepointIsWhitespace, newSegrulesStates);
                if (!newSegrulesStates.empty()
                        && env.getCasePatternHelper().checkInterpsGroupOrthCasePatterns(env, inputStart, currInput, ig)) {
                    for (unsigned int i = 0; i < newSegrulesStates.size(); i++) {
                        const SegrulesState& newSegrulesState = newSegrulesStates[i];
                        const unsigned char* interpsPtr = getInterpretationsPtr(env, ig);
                        const unsigned char* interpsEndPtr = ig.ptr + ig.size;
                        InterpretedChunk ic;
                        ic.segmentType = ig.type;
                        ic.textStartPtr = inputStart;
                        ic.textEndPtr = currInput;
                        ic.interpsGroupPtr = ig.ptr;
                        ic.interpsPtr = interpsPtr;
                        ic.interpsEndPtr = interpsEndPtr;
                        ic.shiftOrth = newSegrulesState.shiftOrthFromPrevious;
                        ic.orthWasShifted = false;
                        ic.requiredHomonymId = homonymId;
                        ic.codepointsNum = codepointsNum;
                        
                        if (!accum.empty() && accum.back().shiftOrth) {
                            doShiftOrth(accum.back(), ic);
                        }
                        accum.push_back(ic);
                        if (currCodepointIsWhitespace) {
                            assert(newSegrulesState.accepting);
                            if (this->options.debug) {
                                cerr << "ACCEPTING " << debugAccum(accum) << endl;
                            }
                            graph.addPath(accum, newSegrulesState.weak);
                        }
                        else {
                            assert(!newSegrulesState.sink);
                            //                        cerr << "will process " << currInput << endl;
                            const char* newCurrInput = currInput;
                            doProcessOneWord(env, newCurrInput, inputEnd, newSegrulesState);
                        }
                        accum.pop_back();
                    }
                }
                else if (this->options.debug) {
//                    cerr << !newSegrulesStates.empty() << env.getCasePatternHelper().checkInterpsGroupOrthCasePatterns(normalizedCodepoints, originalCodepoints, ig) << endl;
                    cerr << "NOT ACCEPTING " << debugAccum(accum) << debugInterpsGroup(ig.type, inputStart, currInput) << endl;
                }
            }
        }
        prevInput = currInput;
        codepoint = currInput == inputEnd || currCodepointIsWhitespace ? 0x00 : env.getCharsetConverter().next(currInput, inputEnd);
    }
    inputData = currInput;
}

void Morfeusz::handleIgnChunk(
        const Environment& env,
        const char* inputStart,
        const char* inputEnd,
        int startNodeNum,
        std::vector<MorphInterpretation>& results) const {
    const char* currInput = inputStart;
    const char* prevInput;
    uint32_t codepoint = 0x00;
    bool separatorFound = false;
    while (currInput != inputEnd) {
        prevInput = currInput;
        const char* nonSeparatorInputEnd = prevInput;
        do {
            codepoint = env.getCharsetConverter().next(currInput, inputEnd);
            if (!env.isSeparator(codepoint)) {
                nonSeparatorInputEnd = currInput;
            }
        }
        while (currInput != inputEnd && !env.isSeparator(codepoint));

        if (env.isSeparator(codepoint)) {
            separatorFound = true;
            if (nonSeparatorInputEnd != prevInput) {
                int startNode = results.empty() ? startNodeNum : results.back().getEndNode();
                this->processOneWord(env, prevInput, nonSeparatorInputEnd, startNode, results, true);
                startNode = results.empty() ? startNodeNum : results.back().getEndNode();
                this->processOneWord(env, nonSeparatorInputEnd, currInput, startNode, results, true);
            }
            else {
                int startNode = results.empty() ? startNodeNum : results.back().getEndNode();
                this->processOneWord(env, prevInput, currInput, startNode, results, true);
            }
        }
    }

    // currInput == inputEnd
    if (!env.isSeparator(codepoint)) {
        if (separatorFound) {
            int startNode = results.empty() ? startNodeNum : results.back().getEndNode();
            this->processOneWord(env, prevInput, inputEnd, startNode, results, true);
        }
        else {
            this->appendIgnotiumToResults(env, string(inputStart, inputEnd), startNodeNum, results);
        }
    }
}

void Morfeusz::appendIgnotiumToResults(
        const Environment& env,
        const string& word,
        int startNodeNum,
        std::vector<MorphInterpretation>& results) const {
    MorphInterpretation interp = MorphInterpretation::createIgn(startNodeNum, word, env);
    results.push_back(interp);
}

ResultsIterator Morfeusz::analyze(const string& text) const {
    vector<MorphInterpretation> res;
    this->analyze(text, res);
    return ResultsIterator(res);
}

void Morfeusz::analyze(const string& text, vector<MorphInterpretation>& results) const {
    const char* input = text.c_str();
    const char* inputEnd = input + text.length();
    while (input != inputEnd) {
        int startNode = results.empty() ? 0 : results.back().getEndNode();
        this->processOneWord(this->analyzerEnv, input, inputEnd, startNode, results);
    }
}

ResultsIterator Morfeusz::generate(const string& text) const {
    vector<MorphInterpretation> res;
    this->generate(text, res);
    return ResultsIterator(res);
}

ResultsIterator Morfeusz::generate(const string& text, int tagnum) const {
    vector<MorphInterpretation> res;
    this->generate(text, tagnum, res);
    return ResultsIterator(res);
}

void Morfeusz::generate(const string& lemma, vector<MorphInterpretation>& results) const {
    const char* input = lemma.c_str();
    const char* inputEnd = input + lemma.length();
    int startNode = 0;
    this->processOneWord(this->generatorEnv, input, inputEnd, startNode, results);
    if (input != inputEnd) {
        throw MorfeuszException("Input contains more than one word");
    }
}

// XXX - someday it should be improved

void Morfeusz::generate(const std::string& lemma, int tagnum, vector<MorphInterpretation>& result) const {
    vector<MorphInterpretation> partRes;
    this->generate(lemma, partRes);
    for (unsigned int i = 0; i < partRes.size(); i++) {
        if (partRes[i].getTagnum() == tagnum) {
            result.push_back(partRes[i]);
        }
    }
}

void Morfeusz::setCharset(MorfeuszCharset charset) {
    this->options.encoding = charset;
    this->analyzerEnv.setCharset(charset);
    this->generatorEnv.setCharset(charset);
}

void Morfeusz::setAggl(const std::string& aggl) {
    this->analyzerEnv.setSegrulesOption("aggl", aggl);
    this->generatorEnv.setSegrulesOption("aggl", aggl);
}

void Morfeusz::setPraet(const std::string& praet) {
    this->analyzerEnv.setSegrulesOption("praet", praet);
    this->generatorEnv.setSegrulesOption("praet", praet);
}

void Morfeusz::setCaseSensitive(bool caseSensitive) {
    this->options.caseSensitive = caseSensitive;
    this->analyzerEnv.setCaseSensitive(caseSensitive);
}

void Morfeusz::setDebug(bool debug) {
    this->options.debug = debug;
}

ResultsIterator::ResultsIterator(const vector<MorphInterpretation>& res) {
    resultsBuffer.insert(resultsBuffer.begin(), res.begin(), res.end());
}

MorphInterpretation ResultsIterator::getNext() {
    MorphInterpretation res = this->resultsBuffer.front();
    this->resultsBuffer.pop_front();
    return res;
}

bool ResultsIterator::hasNext() {
    return !resultsBuffer.empty();
}
