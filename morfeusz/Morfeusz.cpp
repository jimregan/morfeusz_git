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
#include "MorphDeserializer.hpp"
#include "InterpretedChunksDecoder.hpp"
#include "charset/CharsetConverter.hpp"
#include "charset/charset_utils.hpp"
#include "charset/CaseConverter.hpp"
#include "segrules/segrules.hpp"
#include "const.hpp"
#include "deserializationUtils.hpp"
#include "charset/utf8.h"

// TODO - konstruktor kopiujący działający Tak-Jak-Trzeba

using namespace std;

static MorfeuszOptions createDefaultOptions() {
    MorfeuszOptions res;
    res.caseSensitive = true;
    res.encoding = UTF8;
    res.debug = false;
    return res;
}

Morfeusz::Morfeusz()
: analyzerEnv(DEFAULT_MORFEUSZ_CHARSET, ANALYZER, DEFAULT_FSA),
generatorEnv(DEFAULT_MORFEUSZ_CHARSET, GENERATOR, DEFAULT_SYNTH_FSA),
options(createDefaultOptions()) {
    analyzerEnv.setCaseSensitive(options.caseSensitive);
    generatorEnv.setCaseSensitive(false);
}

inline const unsigned char* getInterpretationsPtr(const Environment& env, const InterpsGroup& ig) {
    if (env.getProcessorType() == ANALYZER) {
        const unsigned char* currPtr = ig.ptr;
        unsigned char casePatternsNum = *currPtr++;
        for (unsigned int i = 0; i < casePatternsNum; i++) {
            env.getCasePatternHelper().deserializeOneCasePattern(currPtr);
        }
        return currPtr;
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
            && isEndOfWord(env.getCharsetConverter().peek(inputStart, inputEnd))) {
        env.getCharsetConverter().next(inputStart, inputEnd);
    }
    vector<InterpretedChunk> accum;
    InflexionGraph graph;
    const char* currInput = inputStart;
    const SegrulesFSA& segrulesFSA = env.getCurrentSegrulesFSA();

    doProcessOneWord(env, currInput, inputEnd, segrulesFSA.initialState, accum, graph);

    if (!graph.empty()) {
        const InterpretedChunksDecoder& interpretedChunksDecoder = env.getInterpretedChunksDecoder();
        int srcNode = startNodeNum;
        for (unsigned int i = 0; i < graph.getTheGraph().size(); i++) {
            vector<InflexionGraph::Edge>& edges = graph.getTheGraph()[i];
            for (unsigned int j = 0; j < edges.size(); j++) {
                InflexionGraph::Edge& e = edges[j];
                int targetNode = startNodeNum + e.nextNode;
                interpretedChunksDecoder.decode(srcNode, targetNode, e.chunk, results);
            }
            srcNode++;
        }
    }
    else if (inputStart != inputEnd
            && env.getProcessorType() == ANALYZER
            && !insideIgnHandler) {
        this->handleIgnChunk(env, inputStart, currInput, startNodeNum, results);
        //        this->appendIgnotiumToResults(env, string(inputStart, currInput), startNodeNum, results);
    }
    else if (inputStart != inputEnd) {
        this->appendIgnotiumToResults(env, string(inputStart, currInput), startNodeNum, results);
    }
    inputStart = currInput;
}

static inline void doShiftOrth(InterpretedChunk& from, InterpretedChunk& to) {
    to.prefixChunks.insert(
            to.prefixChunks.begin(),
            from.prefixChunks.begin(),
            from.prefixChunks.end());
    to.prefixChunks.push_back(from);
    from.orthWasShifted = true;
    to.textStartPtr = from.textStartPtr;
}

static inline string debugInterpsGroup(unsigned char type, const char* startPtr, const char* endPtr) {
    stringstream res;
    res << "(" << (int) type << ", " << string(startPtr, endPtr) << "), ";
    return res.str();
}

static inline string debugAccum(vector<InterpretedChunk>& accum) {
    stringstream res;
    for (unsigned int i = 0; i < accum.size(); i++) {
        res << debugInterpsGroup(accum[i].segmentType, accum[i].textStartPtr, accum[i].textEndPtr);
        //        res << "(" << (int) accum[i].interpsGroup.type << ", " << string(accum[i].chunkStartPtr, accum[i].chunkStartPtr) << "), ";
    }
    return res.str();
}

void Morfeusz::doProcessOneWord(
        const Environment& env,
        const char*& inputData,
        const char* inputEnd,
        SegrulesState segrulesState,
        vector<InterpretedChunk>& accum,
        InflexionGraph& graph) const {
    if (this->options.debug) {
        cerr << "----------" << endl;
        cerr << "PROCESS: '" << inputData << "', already recognized: " << debugAccum(accum) << endl;
    }
    //    cerr << "doAnalyzeOneWord " << inputData << endl;
    const char* inputStart = inputData;
    const char* currInput = inputData;
    uint32_t codepoint = inputData == inputEnd ? 0 : env.getCharsetConverter().next(currInput, inputEnd);
    vector<uint32_t> originalCodepoints;
    vector<uint32_t> normalizedCodepoints;

    StateType state = env.getFSA().getInitialState();

    while (!isEndOfWord(codepoint)) {
        uint32_t normalizedCodepoint = env.getProcessorType() == ANALYZER
                ? env.getCaseConverter().toLower(codepoint)
                : codepoint;
        originalCodepoints.push_back(codepoint);
        normalizedCodepoints.push_back(normalizedCodepoint);
        feedState(state, normalizedCodepoint, UTF8CharsetConverter());
        codepoint = currInput == inputEnd ? 0 : env.getCharsetConverter().peek(currInput, inputEnd);
        string homonymId;
        if (env.getProcessorType() == GENERATOR && codepoint == 0x3A && currInput + 1 != inputEnd) {
            if (originalCodepoints.size() == 1) {
                throw MorfeuszException("Lemma of length > 1 cannot start with a colon");
            }
            homonymId = string(currInput + 1, inputEnd);
            //            cerr << "homonym " << homonymId << endl;
            currInput = inputEnd;
            codepoint = 0x00;
        }
        if (state.isAccepting()) {
            vector<InterpsGroup> val(state.getValue());
            for (unsigned int i = 0; i < val.size(); i++) {
                InterpsGroup& ig = val[i];
                if (this->options.debug) {
                    cerr << "recognized: " << debugInterpsGroup(ig.type, inputStart, currInput) << " at: '" << inputStart << "'" << endl;
                }
                set<SegrulesState> newSegrulesStates;
                env.getCurrentSegrulesFSA().proceedToNext(ig.type, segrulesState, newSegrulesStates);
                if (this->options.debug && newSegrulesStates.empty()) {
                    cerr << "NOT ACCEPTING " << debugAccum(accum) << debugInterpsGroup(ig.type, inputStart, currInput) << endl;
                }
                if (!newSegrulesStates.empty() && env.getCasePatternHelper().checkInterpsGroupCasePatterns(normalizedCodepoints, originalCodepoints, ig)) {

                    for (
                            set<SegrulesState>::iterator it = newSegrulesStates.begin();
                            it != newSegrulesStates.end();
                            ++it) {
                        SegrulesState newSegrulesState = *it;
                        const unsigned char* interpsPtr = getInterpretationsPtr(env, ig);
                        const unsigned char* interpsEndPtr = ig.ptr + ig.size;
                        InterpretedChunk ic = {
                            ig.type,
                            inputStart,
                            currInput,
                            originalCodepoints,
                            normalizedCodepoints,
                            interpsPtr,
                            interpsEndPtr,
                            newSegrulesState.shiftOrthFromPrevious,
                            false,
                            vector<InterpretedChunk>(),
                            homonymId
                        };
                        if (!accum.empty() && accum.back().shiftOrth) {
                            doShiftOrth(accum.back(), ic);
                        }
                        accum.push_back(ic);
                        if (isEndOfWord(codepoint)
                                && newSegrulesState.accepting) {
                            if (this->options.debug) {
                                cerr << "ACCEPTING " << debugAccum(accum) << endl;
                            }
                            graph.addPath(accum, newSegrulesState.weak);
                        }
                        else if (!isEndOfWord(codepoint)) {
                            //                        cerr << "will process " << currInput << endl;
                            const char* newCurrInput = currInput;
                            doProcessOneWord(env, newCurrInput, inputEnd, newSegrulesState, accum, graph);
                        }
                        accum.pop_back();
                    }
                }
            }
        }
        codepoint = currInput == inputEnd || isEndOfWord(codepoint) ? 0 : env.getCharsetConverter().next(currInput, inputEnd);
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
    uint32_t codepoint;
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
