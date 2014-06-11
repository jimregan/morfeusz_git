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
        const FSAType& fsa,
        StateType& state,
        const char* inputStart,
        const char* inputEnd) {
    const char* currInput = inputStart;
    while (currInput != inputEnd && !state.isSink()) {
        state.proceedToNext(fsa, *currInput++);
    }
}

static void feedStateIndirectly(
        const FSAType& fsa,
        StateType& state,
        uint32_t codepoint) {
    std::string chars;
    UTF8CharsetConverter::getInstance().append(codepoint, chars);
    for (unsigned int i = 0; i < chars.length() && !state.isSink(); i++) {
        state.proceedToNext(fsa, chars[i]);
    }
}

static void feedState(
        const Environment& env,
        StateType& state,
        TextReader& reader) {
    if (reader.peek() == reader.normalizedPeek() && &env.getCharsetConverter() == &UTF8CharsetConverter::getInstance()) {
        feedStateDirectly(env.getFSA(), state, reader.getCurrPtr(), reader.getNextPtr());
    }
    else {
        feedStateIndirectly(env.getFSA(), state, reader.normalizedPeek());
    }
}

static InterpretedChunk createChunk(
        const InterpsGroup& ig,
        const TextReader& reader,
        bool shiftOrth,
        const string& homonymId) {
    const unsigned char* interpsEndPtr = ig.ptr + ig.size;
    InterpretedChunk ic;
    ic.segmentType = ig.type;
    ic.textStartPtr = reader.getWordStartPtr();
    ic.textEndPtr = reader.getCurrPtr();
    ic.interpsGroupPtr = ig.ptr;
    ic.interpsEndPtr = interpsEndPtr;
    ic.shiftOrth = shiftOrth;
    ic.orthWasShifted = false;
    ic.requiredHomonymId = homonymId;
    ic.codepointsNum = reader.getCodepointsRead();
    return ic;
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
        TextReader& reader,
        int startNodeNum,
        std::vector<MorphInterpretation>& results,
        bool insideIgnHandler) const {

    reader.skipWhitespaces();

    if (reader.isAtEnd()) {
        return;
    }
    accum.clear();
    graph.clear();

    const SegrulesFSA& segrulesFSA = env.getCurrentSegrulesFSA();

    reader.markWordStartsHere();
    doProcessOneWord(env, reader, segrulesFSA.initialState);

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
    else if (env.getProcessorType() == ANALYZER
            && !insideIgnHandler) {
        this->handleIgnChunk(env, reader.getWordStartPtr(), reader.getCurrPtr(), startNodeNum, results);
    }
    else {
        this->appendIgnotiumToResults(env, string(reader.getWordStartPtr(), reader.getCurrPtr()), startNodeNum, results);
    }
}

void Morfeusz::doProcessOneWord(
        const Environment& env,
        TextReader& reader,
        const SegrulesState& segrulesState) const {
    if (this->options.debug) {
        cerr << "----------" << endl;
        cerr << "PROCESS: '" << reader.getCurrPtr() << "', already recognized: " << debugAccum(accum) << endl;
    }
    vector<SegrulesState> newSegrulesStates;

    StateType state = env.getFSA().getInitialState();

    while (!reader.isAtWhitespace()) {
        string homonymId;
        if (env.getProcessorType() == GENERATOR && reader.peek() == 0x3A && reader.getCurrPtr() + 1 != reader.getEndPtr()) {
            homonymId = string(reader.getCurrPtr() + 1, reader.getEndPtr());
            reader.proceedToEnd();
        }
        else {
            feedState(env, state, reader);
            reader.next();
        }
        if (state.isAccepting()) {
            InterpsGroupsReader& igReader = const_cast<InterpsGroupsReader&> (state.getValue());
            while (igReader.hasNext()) {
                const InterpsGroup& ig = igReader.getNext();
                if (this->options.debug) {
                    cerr << "recognized: " << debugInterpsGroup(ig.type, reader.getWordStartPtr(), reader.getCurrPtr()) << " at: '" << reader.getWordStartPtr() << "'" << endl;
                }
                newSegrulesStates.clear();
                env.getCurrentSegrulesFSA().proceedToNext(ig.type, segrulesState, reader.isAtWhitespace(), newSegrulesStates);
                if (!newSegrulesStates.empty()
                        && env.getCasePatternHelper().checkInterpsGroupOrthCasePatterns(env, reader.getWordStartPtr(), reader.getCurrPtr(), ig)) {
                    for (unsigned int i = 0; i < newSegrulesStates.size(); i++) {
                        const SegrulesState& newSegrulesState = newSegrulesStates[i];

                        InterpretedChunk ic(
                                createChunk(ig, reader, newSegrulesState.shiftOrthFromPrevious, homonymId));

                        if (!accum.empty() && accum.back().shiftOrth) {
                            doShiftOrth(accum.back(), ic);
                        }
                        accum.push_back(ic);
                        if (reader.isAtWhitespace()) {
                            assert(newSegrulesState.accepting);
                            if (this->options.debug) {
                                cerr << "ACCEPTING " << debugAccum(accum) << endl;
                            }
                            graph.addPath(accum, newSegrulesState.weak);
                        }
                        else {
                            assert(!newSegrulesState.sink);
                            TextReader newReader(reader.getCurrPtr(), reader.getEndPtr(), env);
                            doProcessOneWord(env, newReader, newSegrulesState);
                        }
                        accum.pop_back();
                    }
                }
                else if (this->options.debug) {
                    //                    cerr << !newSegrulesStates.empty() << env.getCasePatternHelper().checkInterpsGroupOrthCasePatterns(normalizedCodepoints, originalCodepoints, ig) << endl;
                    cerr << "NOT ACCEPTING " << debugAccum(accum) << debugInterpsGroup(ig.type, reader.getWordStartPtr(), reader.getCurrPtr()) << endl;
                }
            }
        }
    }
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
                TextReader newReader1(prevInput, nonSeparatorInputEnd, env);
                this->processOneWord(env, newReader1, startNode, results, true);

                startNode = results.empty() ? startNodeNum : results.back().getEndNode();
                TextReader newReader2(nonSeparatorInputEnd, currInput, env);
                this->processOneWord(env, newReader2, startNode, results, true);
            }
            else {
                int startNode = results.empty() ? startNodeNum : results.back().getEndNode();
                TextReader newReader3(prevInput, currInput, env);
                this->processOneWord(env, newReader3, startNode, results, true);
            }
        }
    }

    // currInput == inputEnd
    if (!env.isSeparator(codepoint)) {
        if (separatorFound) {
            int startNode = results.empty() ? startNodeNum : results.back().getEndNode();
            TextReader newReader4(prevInput, inputEnd, env);
            this->processOneWord(env, newReader4, startNode, results, true);
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
    TextReader reader(input, inputEnd, this->analyzerEnv);
    while (!reader.isAtEnd()) {
        int startNode = results.empty() ? 0 : results.back().getEndNode();
        this->processOneWord(this->analyzerEnv, reader, startNode, results);
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
    TextReader reader(input, inputEnd, this->generatorEnv);
    this->processOneWord(this->generatorEnv, reader, startNode, results);
    if (reader.getCurrPtr() != reader.getEndPtr()) {
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
