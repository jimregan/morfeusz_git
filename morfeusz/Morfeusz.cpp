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

// TODO - konstruktor kopiujący działający Tak-Jak-Trzeba

using namespace std;

static Deserializer<vector<InterpsGroup>>* initializeDeserializer() {
    static Deserializer < vector < InterpsGroup >> *deserializer
            = new MorphDeserializer();
    return deserializer;
}

static FSA<vector<InterpsGroup >> *initializeFSA(const string& filename) {
    cerr << "initialize FSA" << endl;
    return FSA < vector < InterpsGroup >> ::getFSA(filename, *initializeDeserializer());
}

static CharsetConverter* initializeCharsetConverter() {
    cerr << "initialize charset converter" << endl;
    static CharsetConverter* converter = new UTF8CharsetConverter();
    return converter;
}

static Tagset* initializeTagset(const string& filename) {
    cerr << "initialize tagset" << endl;
    static Tagset* tagset = new Tagset(readFile(filename.c_str()));
    return tagset;
}

static Tagset* initializeTagset(const unsigned char* data) {
    cerr << "initialize tagset" << endl;
    static Tagset* tagset = new Tagset(data);
    return tagset;
}

static CaseConverter* initializeCaseConverter() {
    cerr << "initialize case converter" << endl;
    static CaseConverter* cc = new CaseConverter();
    return cc;
}

Morfeusz::Morfeusz()
: fsa(FSAType::getFSA(DEFAULT_FSA, *initializeDeserializer())),
charsetConverter(initializeCharsetConverter()),
tagset(initializeTagset(DEFAULT_FSA)),
caseConverter(initializeCaseConverter()),
caseSensitive(true) {

}

Morfeusz::Morfeusz(const string& filename)
: fsa(initializeFSA(filename)),
charsetConverter(initializeCharsetConverter()),
tagset(initializeTagset(filename)),
caseConverter(initializeCaseConverter()),
caseSensitive(true) {

}

Morfeusz::~Morfeusz() {
    //    delete &this->fsa;
    //    delete &this->charsetConverter;
}

void Morfeusz::processOneWord(
        const char*& inputData,
        const char* inputEnd,
        int startNodeNum,
        std::vector<MorphInterpretation>& results) const {
    while (inputData != inputEnd
            && isEndOfWord(this->charsetConverter->peek(inputData, inputEnd))) {
        this->charsetConverter->next(inputData, inputEnd);
    }
    const char* wordStart = inputData;
    DEBUG(wordStart);
    vector<InterpretedChunk> accum;
    FlexionGraph graph;
    const char* currInput = inputData;
    doProcessOneWord(currInput, inputEnd, accum, graph);
    if (!graph.empty()) {
        InterpretedChunksDecoder interpretedChunksDecoder(*tagset, *charsetConverter, *caseConverter);
        int srcNode = startNodeNum;
        for (vector<FlexionGraph::Edge>& edges : graph.getTheGraph()) {
            for (FlexionGraph::Edge& e : edges) {
                int targetNode = startNodeNum + e.nextNode;
                interpretedChunksDecoder.decode(srcNode, targetNode, e.chunk, back_inserter(results));
            }
            srcNode++;
        }
        //        graph.getResults(*this->tagset, results);
    } else if (wordStart != currInput) {
        this->appendIgnotiumToResults(string(wordStart, currInput), startNodeNum, results);
    }
    inputData = currInput;
}

void Morfeusz::doProcessOneWord(
        const char*& inputData,
        const char* inputEnd,
        vector<InterpretedChunk>& accum,
        FlexionGraph& graph) const {
    bool endOfWord = inputData == inputEnd;
    const char* currInput = inputData;
    uint32_t codepoint = endOfWord ? 0 : this->charsetConverter->next(currInput, inputEnd);
    //    UnicodeChunk uchunk(*(this->charsetConverter), *(this->caseConverter));
    vector<uint32_t> originalCodepoints;
    vector<uint32_t> lowercaseCodepoints;

    StateType state = this->fsa->getInitialState();

    while (!isEndOfWord(codepoint)) {
        uint32_t lowerCP = this->caseConverter->toLower(codepoint);
        originalCodepoints.push_back(codepoint);
        lowercaseCodepoints.push_back(lowerCP);
        this->feedState(state, lowerCP);
        if (state.isAccepting()) {
            for (InterpsGroup& ig : state.getValue()) {
                //                for (EncodedInterpretation& ei: ig.interps) {
                //                    cerr << "CUT: " << ei.lemma.suffixToCut << "; ADD: " << ei.lemma.suffixToAdd << endl;
                //                }
                InterpretedChunk ic = {inputData, originalCodepoints, lowercaseCodepoints, ig};
                accum.push_back(ic);
                const char* newCurrInput = currInput;
                doProcessOneWord(newCurrInput, inputEnd, accum, graph);
                accum.pop_back();
            }
        }
        codepoint = currInput == inputEnd ? 0 : this->charsetConverter->peek(currInput, inputEnd);
        if (!isEndOfWord(codepoint)) {
            this->charsetConverter->next(currInput, inputEnd);
        }
    }
    if (state.isAccepting()) {
        for (InterpsGroup& ig : state.getValue()) {
            InterpretedChunk ic = {inputData, originalCodepoints, lowercaseCodepoints, ig};
            accum.push_back(ic);
            graph.addPath(accum);
            accum.pop_back();
        }
    }
    inputData = currInput;
}

void Morfeusz::feedState(
        StateType& state,
        int codepoint) const {
    string chars;
    this->charsetConverter->append(codepoint, chars);
    for (char c : chars) {
        state.proceedToNext(c);
    }
}

void Morfeusz::appendIgnotiumToResults(
        const string& word,
        int startNodeNum,
        std::vector<MorphInterpretation>& results) const {
    MorphInterpretation interp = MorphInterpretation::createIgn(startNodeNum, word, *this->tagset);
    results.push_back(interp);
}

ResultsIterator Morfeusz::analyze(const string& text) const {
    //    const char* textStart = text.c_str();
    //    const char* textEnd = text.c_str() + text.length();
    return ResultsIterator(text, *this);
}

void Morfeusz::analyze(const string& text, vector<MorphInterpretation>& results) const {
    const char* input = text.c_str();
    const char* inputEnd = input + text.length();
    while (input != inputEnd) {
        int startNode = results.empty() ? 0 : results.back().getEndNode();
        DEBUG("process " + string(input, inputEnd));
        this->processOneWord(input, inputEnd, startNode, results);
    }
}

ResultsIterator::ResultsIterator(const string& text, const Morfeusz& morfeusz)
: rawInput(text.c_str()),
morfeusz(morfeusz) {
}

MorphInterpretation ResultsIterator::getNext() {
    //    if (resultsBuffer.empty()) {
    //        morfeusz.processOneWord(rawInput, startNode, back_inserter(resultsBuffer));
    //    }
    //    startNode = resultsBuffer.back().getEndNode();
    //    MorphInterpretation res = resultsBuffer.front();
    //    resultsBuffer.pop_front();
    //    return res;
}

bool ResultsIterator::hasNext() {
    return rawInput[0] != '\0' && resultsBuffer.empty();
}

//int Morfeusz::doProcessOneWord(const char*& inputPtr, const char* inputEnd, int startNodeNum, std::vector<EncodedInterpretation>& interps) const {
//    assert(inputPtr[0] != '\0');
//    const char* start = inputPtr;
//    StateType state = fsa->getInitialState();
//    int currNodeNum = startNodeNum;
//    int codepoint = this->charsetConverter->next(inputPtr, inputEnd);
//    assert(!isEndOfWord(codepoint));
//    while(!isEndOfWord(codepoint)) {
//        feedState(state, codepoint);
//        if (state.isAccepting()) {
//            const char* currInputPtr = inputPtr;
//            vector<EncodedInterpretation> startInterps = state.getValue();
//            filterOutNonGluableInterps(startInterps);
//            if (!startInterps.empty()) {
//                
//            }
//            vector<EncodedInterpretation> additionalInterps;
//            int nextNodeNum = doProcessOneWord(currInputPtr, inputEnd, currNodeNum + 1, additionalInterps);
//            if (!additionalInterps.empty()) {
//                for (EncodedInterpretation& interp: state.getValue()) {
//                    interp.startNode = currNodeNum;
//                    interp.endNode = currNodeNum + 1;
//                    interps.push_back(interp);
//                }
//                
//            }
//        }
//    }
//}
