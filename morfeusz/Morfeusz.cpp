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

//Morfeusz::Morfeusz(const Morfeusz& orig) {
//}

Morfeusz::~Morfeusz() {
    delete &this->fsa;
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
