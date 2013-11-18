/* 
 * File:   Morfeusz.cpp
 * Author: mlenart
 * 
 * Created on November 13, 2013, 5:21 PM
 */

#include <string>
#include "fsa.hpp"
#include "utils.hpp"
#include "Morfeusz.hpp"
#include "MorphDeserializer.hpp"
#include "charset/CharsetConverter.hpp"
#include "charset/charset_utils.hpp"

// TODO - konstruktor kopiujący działający Tak-Jak-Trzeba

using namespace std;

static FSA<vector<InterpsGroup >> *initializeFSA(const string& filename) {
    static Deserializer < vector < InterpsGroup >> *deserializer
            = new MorphDeserializer();
    return FSA < vector < InterpsGroup >> ::getFSA(filename, *deserializer);
}

static CharsetConverter* initializeCharsetConverter() {
    static CharsetConverter* converter = new UTF8CharsetConverter();
    return converter;
}

Morfeusz::Morfeusz(const string& filename)
: fsa(initializeFSA(filename)), charsetConverter(initializeCharsetConverter()) {

}

Morfeusz::~Morfeusz() {
    delete &this->fsa;
    delete &this->charsetConverter;
}

void Morfeusz::processOneWord(
        const char*& inputData,
        const char* inputEnd,
        const int startNodeNum,
        std::vector<MorphInterpretation>& results) const {
    vector<InterpretedChunk> accum;
    FlexionGraph graph(startNodeNum);
    const char* currInput = inputData;
    doProcessOneWord(currInput, inputEnd, accum, graph);
    graph.appendToResults(this->tagset, results);
    inputData = currInput;
}

void Morfeusz::doProcessOneWord(
        const char*& inputData,
        const char* inputEnd,
        vector<InterpretedChunk>& accum,
        FlexionGraph& graph) const {
    const char* currInput = inputData;
    StateType state = this->fsa->getInitialState();
    int codepoint = this->charsetConverter->next(currInput, inputEnd);
    
    if (!accum.empty() && isEndOfWord(codepoint)) {
        graph.addPath(accum);
    }
    else
        while (!isEndOfWord(codepoint)) {
            this->feedState(state, codepoint);
            codepoint = this->charsetConverter->next(currInput, inputEnd);
            if (state.isAccepting()) {
                for (InterpsGroup& ig : state.getValue()) {
                    InterpretedChunk ic = {inputData, currInput - inputData, ig};
                    accum.push_back(ic);
                    doProcessOneWord(currInput, inputEnd, accum, graph);
                    accum.pop_back();
                }
            }
        }
}

void Morfeusz::feedState(
        StateType& state,
        const int codepoint) const {
    vector<char> chars;
    this->charsetConverter->append(codepoint, chars);
    for (char c: chars) {
        state.proceedToNext(c);
    }
}

ResultsIterator Morfeusz::analyze(const std::string& text) {
    //    const char* textStart = text.c_str();
    //    const char* textEnd = text.c_str() + text.length();
    return ResultsIterator(text, *this);
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
