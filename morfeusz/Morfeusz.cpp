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
#include "GeneratorDeserializer.hpp"
#include "InterpretedChunksDecoder.hpp"
#include "charset/CharsetConverter.hpp"
#include "charset/charset_utils.hpp"
#include "charset/CaseConverter.hpp"
#include "segrules/segrules.hpp"
#include "const.hpp"

// TODO - konstruktor kopiujący działający Tak-Jak-Trzeba

using namespace std;

static MorfeuszOptions createDefaultOptions() {
    MorfeuszOptions res;
    res.caseSensitive = true;
    res.encoding = UTF8;
    return res;
}

Morfeusz::Morfeusz()
: analyzerEnv(DEFAULT_MORFEUSZ_CHARSET, ANALYZER, DEFAULT_FSA),
generatorEnv(DEFAULT_MORFEUSZ_CHARSET, GENERATOR, DEFAULT_SYNTH_FSA),
options(createDefaultOptions()) {

}

void Morfeusz::setAnalyzerFile(const string& filename) {
    this->analyzerEnv.setFSAFile(filename);
    //    if (this->isAnalyzerFSAFromFile) {
    //        delete this->analyzerFSA;
    //        deleteSegrulesFSAs(this->analyzerSegrulesFSAsMap);
    //        delete this->analyzerPtr;
    //    }
    //    this->analyzerPtr = readFile<unsigned char>(filename.c_str());
    //    this->analyzerFSA = FSA< vector<InterpsGroup> > ::getFSA(analyzerPtr, *initializeAnalyzerDeserializer());
    //    this->analyzerSegrulesFSAsMap = createSegrulesFSAsMap(analyzerPtr);
    //    this->isAnalyzerFSAFromFile = true;
}

void Morfeusz::setGeneratorFile(const string& filename) {
    this->generatorEnv.setFSAFile(filename);
    //    if (this->isGeneratorFSAFromFile) {
    //        delete this->generatorPtr;
    //    }
    //    this->generatorPtr = readFile<unsigned char>(filename.c_str());
    //    this->generator.setGeneratorPtr(generatorPtr);
}

Morfeusz::~Morfeusz() {
    //    if (this->isAnalyzerFSAFromFile) {
    //        delete this->analyzerFSA;
    //        deleteSegrulesFSAs(this->analyzerSegrulesFSAsMap);
    //        delete this->analyzerPtr;
    //    }
}

void Morfeusz::processOneWord(
        const Environment& env,
        const char*& inputStart,
        const char* inputEnd,
        int startNodeNum,
        std::vector<MorphInterpretation>& results) const {
    while (inputStart != inputEnd
            && isEndOfWord(env.getCharsetConverter().peek(inputStart, inputEnd))) {
        env.getCharsetConverter().next(inputStart, inputEnd);
    }
    vector<InterpretedChunk> accum;
    FlexionGraph graph;
    const char* currInput = inputStart;
    const SegrulesFSA& segrulesFSA = env.getCurrentSegrulesFSA();
    
    doProcessOneWord(env, currInput, inputEnd, segrulesFSA.initialState, accum, graph);
    
    if (!graph.empty()) {
        const InterpretedChunksDecoder& interpretedChunksDecoder = env.getInterpretedChunksDecoder();
        int srcNode = startNodeNum;
        for (unsigned int i = 0; i < graph.getTheGraph().size(); i++) {
            vector<FlexionGraph::Edge>& edges = graph.getTheGraph()[i];
            for (unsigned int j = 0; j < edges.size(); j++) {
                FlexionGraph::Edge& e = edges[j];
                int targetNode = startNodeNum + e.nextNode;
                interpretedChunksDecoder.decode(srcNode, targetNode, e.chunk, results);
            }
            srcNode++;
        }
        //        graph.getResults(*this->tagset, results);
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
    to.chunkStartPtr = from.chunkStartPtr;
}

void Morfeusz::doProcessOneWord(
        const Environment& env,
        const char*& inputData,
        const char* inputEnd,
        SegrulesState segrulesState,
        vector<InterpretedChunk>& accum,
        FlexionGraph& graph) const {
//    cerr << "doAnalyzeOneWord " << inputData << endl;
    const char* currInput = inputData;
    uint32_t codepoint = inputData == inputEnd ? 0 : env.getCharsetConverter().next(currInput, inputEnd);
    vector<uint32_t> originalCodepoints;
    vector<uint32_t> lowercaseCodepoints;

    StateType state = env.getFSA().getInitialState();

    while (!isEndOfWord(codepoint)) {
        uint32_t lowerCP = env.getCaseConverter().toLower(codepoint);
        originalCodepoints.push_back(codepoint);
        lowercaseCodepoints.push_back(lowerCP);
        feedState(state, lowerCP, UTF8CharsetConverter());
        codepoint = currInput == inputEnd ? 0 : env.getCharsetConverter().peek(currInput, inputEnd);
        if (state.isAccepting()) {
            vector<InterpsGroup> val(state.getValue());
            for (unsigned int i = 0; i < val.size(); i++) {
                InterpsGroup& ig = val[i];
                set<SegrulesState> newSegrulesStates;
                env.getCurrentSegrulesFSA().proceedToNext(ig.type, segrulesState, newSegrulesStates);
                for (
                        set<SegrulesState>::iterator it = newSegrulesStates.begin();
                        it != newSegrulesStates.end();
                        ++it) {
                    SegrulesState newSegrulesState = *it;
//                    if (newSegrulesState.shiftOrthFromPrevious) {
//                        
//                    }
                    InterpretedChunk ic = {
                        inputData,
                        originalCodepoints,
                        lowercaseCodepoints,
                        ig,
                        newSegrulesState.shiftOrthFromPrevious,
                        false,
                        vector<InterpretedChunk>()
                    };
                    if (!accum.empty() && accum.back().shiftOrth) {
                        doShiftOrth(accum.back(), ic);
                    }
                    accum.push_back(ic);
                    if (isEndOfWord(codepoint)) {
                        if (newSegrulesState.accepting)
                            graph.addPath(accum);
                    }
                    else {
                        const char* newCurrInput = currInput;
                        doProcessOneWord(env, newCurrInput, inputEnd, newSegrulesState, accum, graph);
                    }
                    accum.pop_back();
                }
            }
        }
        codepoint = currInput == inputEnd ? 0 : env.getCharsetConverter().next(currInput, inputEnd);
    }
    inputData = currInput;
}

void Morfeusz::appendIgnotiumToResults(
        const Environment& env,
        const string& word,
        int startNodeNum,
        std::vector<MorphInterpretation>& results) const {
    MorphInterpretation interp = MorphInterpretation::createIgn(startNodeNum, word, env.getTagset(), env.getCharsetConverter());
    results.push_back(interp);
}

ResultsIterator Morfeusz::analyze(const string& text) const {
    //    const char* textStart = text.c_str();
    //    const char* textEnd = text.c_str() + text.length();
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
    //    const char* textStart = text.c_str();
    //    const char* textEnd = text.c_str() + text.length();
    vector<MorphInterpretation> res;
    this->generate(text, res);
    return ResultsIterator(res);
}

void Morfeusz::generate(const string& text, vector<MorphInterpretation>& results) const {
    const char* input = text.c_str();
    const char* inputEnd = input + text.length();
    while (input != inputEnd) {
        int startNode = results.empty() ? 0 : results.back().getEndNode();
        this->processOneWord(this->generatorEnv, input, inputEnd, startNode, results);
    }
}

void Morfeusz::setCharset(MorfeuszCharset charset) {
    this->options.encoding = charset;
    this->analyzerEnv.setCharset(charset);
    this->generatorEnv.setCharset(charset);
}

ResultsIterator::ResultsIterator(vector<MorphInterpretation>& res) {
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
