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

static Deserializer<vector<InterpsGroup> >* initializeAnalyzerDeserializer() {
    static Deserializer < vector < InterpsGroup > > *deserializer
            = new MorphDeserializer();
    return deserializer;
}

static MorfeuszOptions createDefaultOptions() {
    MorfeuszOptions res;
    res.caseSensitive = true;
    res.encoding = UTF8;
    return res;
}

Morfeusz::Morfeusz()
: env(Tagset(DEFAULT_FSA), Tagset(DEFAULT_SYNTH_FSA), DEFAULT_MORFEUSZ_CHARSET),
analyzerPtr(DEFAULT_FSA),
analyzerFSA(FSAType::getFSA(analyzerPtr, *initializeAnalyzerDeserializer())),
segrulesFSAsMap(createSegrulesFSAsMap(analyzerPtr)),
isAnalyzerFSAFromFile(false),
generatorPtr(DEFAULT_SYNTH_FSA),
isGeneratorFSAFromFile(false),
generator(generatorPtr, env),
options(createDefaultOptions()) {

}

static void deleteSegrulesFSAs(std::map<SegrulesOptions, SegrulesFSAType*>& fsasMap) {
    for (
            std::map<SegrulesOptions, SegrulesFSAType*>::iterator it = fsasMap.begin();
            it != fsasMap.end();
            ++it) {
        delete it->second;
    }
    fsasMap.clear();
}

void Morfeusz::setAnalyzerFile(const string& filename) {
    if (this->isAnalyzerFSAFromFile) {
        delete this->analyzerFSA;
        deleteSegrulesFSAs(this->segrulesFSAsMap);
        delete this->analyzerPtr;
    }
    this->analyzerPtr = readFile<unsigned char>(filename.c_str());
    this->analyzerFSA = FSA< vector<InterpsGroup> > ::getFSA(analyzerPtr, *initializeAnalyzerDeserializer());
    this->segrulesFSAsMap = createSegrulesFSAsMap(analyzerPtr);
    this->isAnalyzerFSAFromFile = true;
}

void Morfeusz::setGeneratorFile(const string& filename) {
    if (this->isGeneratorFSAFromFile) {
        delete this->generatorPtr;
    }
    this->generatorPtr = readFile<unsigned char>(filename.c_str());
    this->generator.setGeneratorPtr(generatorPtr);
}

Morfeusz::~Morfeusz() {
    if (this->isAnalyzerFSAFromFile) {
        delete this->analyzerFSA;
        deleteSegrulesFSAs(this->segrulesFSAsMap);
        delete this->analyzerPtr;
    }
}

void Morfeusz::analyzeOneWord(
        const char*& inputStart,
        const char* inputEnd,
        int startNodeNum,
        std::vector<MorphInterpretation>& results) const {
    while (inputStart != inputEnd
            && isEndOfWord(this->env.getCharsetConverter().peek(inputStart, inputEnd))) {
        this->env.getCharsetConverter().next(inputStart, inputEnd);
    }
    vector<InterpretedChunk> accum;
    FlexionGraph graph;
    const char* currInput = inputStart;
    SegrulesOptions opts;
    opts["aggl"] = "isolated";
    opts["praet"] = "split";
    SegrulesFSAType* segrulesFSA = (*(this->segrulesFSAsMap.find(opts))).second;
    doAnalyzeOneWord(currInput, inputEnd, accum, graph, segrulesFSA->getInitialState());
    if (!graph.empty()) {
        InterpretedChunksDecoder interpretedChunksDecoder(env);
        int srcNode = startNodeNum;
        for (unsigned int i = 0; i < graph.getTheGraph().size(); i++) {
            vector<FlexionGraph::Edge>& edges = graph.getTheGraph()[i];
            for (unsigned int j = 0; j < edges.size(); j++) {
                FlexionGraph::Edge& e = edges[j];
                int targetNode = startNodeNum + e.nextNode;
                interpretedChunksDecoder.decode(srcNode, targetNode, e.chunk, back_inserter(results));
            }
            srcNode++;
        }
        //        graph.getResults(*this->tagset, results);
    } else if (inputStart != inputEnd) {
        this->appendIgnotiumToResults(string(inputStart, currInput), startNodeNum, results);
    }
    inputStart = currInput;
}

void Morfeusz::doAnalyzeOneWord(
        const char*& inputData,
        const char* inputEnd,
        vector<InterpretedChunk>& accum,
        FlexionGraph& graph,
        SegrulesStateType segrulesState) const {
    bool endOfWord = inputData == inputEnd;
    const char* currInput = inputData;
    uint32_t codepoint = endOfWord ? 0 : this->env.getCharsetConverter().next(currInput, inputEnd);
    //    UnicodeChunk uchunk(*(this->charsetConverter), *(this->caseConverter));
    vector<uint32_t> originalCodepoints;
    vector<uint32_t> lowercaseCodepoints;

    StateType state = this->analyzerFSA->getInitialState();

    while (!isEndOfWord(codepoint)) {
        uint32_t lowerCP = this->env.getCaseConverter().toLower(codepoint);
        originalCodepoints.push_back(codepoint);
        lowercaseCodepoints.push_back(lowerCP);
        feedState(state, lowerCP, UTF8CharsetConverter());
        codepoint = currInput == inputEnd ? 0 : this->env.getCharsetConverter().peek(currInput, inputEnd);
        if (!isEndOfWord(codepoint)) {
            if (state.isAccepting()) {
                vector<InterpsGroup> val(state.getValue());
                for (unsigned int i = 0; i < val.size(); i++) {
                    InterpsGroup& ig = val[i];

                    SegrulesStateType newSegrulesState = segrulesState;
                    newSegrulesState.proceedToNext(ig.type);
                    if (!newSegrulesState.isSink()) {
                        bool shiftOrth = newSegrulesState.getLastTransitionValue();
                        InterpretedChunk ic = {inputData, originalCodepoints, lowercaseCodepoints, ig, shiftOrth};
                        if (!accum.empty() && accum.back().shiftOrth) {
                            ic.originalCodepoints.insert(
                                    ic.originalCodepoints.begin(),
                                    accum.back().originalCodepoints.begin(),
                                    accum.back().originalCodepoints.end());
                            ic.chunkStartPtr = accum.back().chunkStartPtr;
                        }
                        accum.push_back(ic);
                        const char* newCurrInput = currInput;
                        doAnalyzeOneWord(newCurrInput, inputEnd, accum, graph, newSegrulesState);
                        accum.pop_back();
                    }
                }
            }

            this->env.getCharsetConverter().next(currInput, inputEnd);
        }
    }
    // we are at the end of word
    if (state.isAccepting()) {
        vector<InterpsGroup > val(state.getValue());
        for (unsigned int i = 0; i < val.size(); i++) {
            InterpsGroup& ig = val[i];
            SegrulesStateType newSegrulesState = segrulesState;
            newSegrulesState.proceedToNext(ig.type);
            if (newSegrulesState.isAccepting()) {
                bool shiftOrth = newSegrulesState.getLastTransitionValue();
                InterpretedChunk ic = {inputData, originalCodepoints, lowercaseCodepoints, ig, shiftOrth};
                if (!accum.empty() && accum.back().shiftOrth) {
                    ic.originalCodepoints.insert(
                            ic.originalCodepoints.begin(),
                            accum.back().originalCodepoints.begin(),
                            accum.back().originalCodepoints.end());
                    ic.chunkStartPtr = accum.back().chunkStartPtr;
                }
                accum.push_back(ic);
                graph.addPath(accum);
                accum.pop_back();
            } else if (!newSegrulesState.isSink()) {
            } else {
            }
        }
    }
    inputData = currInput;
}

void Morfeusz::appendIgnotiumToResults(
        const string& word,
        int startNodeNum,
        std::vector<MorphInterpretation>& results) const {
    MorphInterpretation interp = MorphInterpretation::createIgn(startNodeNum, word, env.getAnalyzerTagset(), env.getCharsetConverter());
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
        this->analyzeOneWord(input, inputEnd, startNode, results);
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
    this->generator.generate(text, results);
}

void Morfeusz::setCharset(MorfeuszCharset charset) {
    this->options.encoding = charset;
    this->env.setCharset(charset);
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
