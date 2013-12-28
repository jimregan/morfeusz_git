/* 
 * File:   MorphDeserializer.cpp
 * Author: mlenart
 * 
 * Created on 12 listopad 2013, 15:31
 */

#include <map>
#include <algorithm>
//#include <cstdint>
#include "MorphDeserializer.hpp"
#include "EncodedInterpretation.hpp"
#include "InterpsGroup.hpp"

const uint8_t LEMMA_ONLY_LOWER = 0;
const uint8_t LEMMA_UPPER_PREFIX = 1;
const uint8_t LEMMA_MIXED_CASE = 2;
const unsigned int MAX_WORD_SIZE = 256;

MorphDeserializer::MorphDeserializer() {
}

MorphDeserializer::~MorphDeserializer() {
}

static void deserializeLemma(const unsigned char*& ptr, EncodedLemma& lemma) {
    // XXX uważać na poprawność danych
    lemma.suffixToCut = *ptr;
    ptr++;
    lemma.suffixToAdd = (const char*) ptr;
    ptr += strlen((const char*) ptr) + 1;
    assert(lemma.casePattern.size() == 0);
    lemma.casePattern.resize(MAX_WORD_SIZE, false);
    uint8_t casePatternType = *ptr;
    ptr++;
    uint8_t prefixLength;
    uint8_t patternLength;
    switch (casePatternType) {
        case LEMMA_ONLY_LOWER:
            break;
        case LEMMA_UPPER_PREFIX:
            prefixLength = *ptr;
            ptr++;
            for (unsigned int i = 0; i < prefixLength; i++) {
                lemma.casePattern[i] = true;
            }
//            lemma.casePattern.resize(prefixLength, true);
            break;
        case LEMMA_MIXED_CASE:
            patternLength = *ptr;
            ptr++;
            for (unsigned int i = 0; i < patternLength; i++) {
                uint8_t idx = *ptr;
                ptr++;
//                lemma.casePattern.resize(max(lemma.casePattern.size(), (unsigned long) idx + 1), false);
                lemma.casePattern[idx] = true;
            }
            break;
    }
}

static void deserializeInterp(const unsigned char*& ptr, EncodedInterpretation& interp) {
    interp.type = *ptr;
    ptr++;
    deserializeLemma(ptr, interp.lemma);
    interp.tag = ntohs(*(reinterpret_cast<const uint16_t*>(ptr)));
    ptr += 2;
    interp.nameClassifier = *ptr;
    ptr++;
}

long MorphDeserializer::deserialize(const unsigned char* ptr, vector<InterpsGroup>& interps) const {
    const unsigned char* currPtr = ptr;
    uint8_t interpsNum = *ptr;
    interps.clear();
    interps.reserve(interpsNum);
    currPtr++;
    // FIXME - to jest do poprawy
    map<int, InterpsGroup> results;
    for (unsigned int i = 0; i < interpsNum; ++i) {
        EncodedInterpretation interp;
        deserializeInterp(currPtr, interp);
        if (results.count(interp.type) == 0) {
            results[interp.type] = InterpsGroup(interp.type);
        }
        results[interp.type].addInterpretation(interp);
//        interps.push_back(interp);
    }
    map<int, InterpsGroup>::iterator it;
    for (it = results.begin(); it != results.end(); ++it) {
        interps.push_back((*it).second);
    }
    return currPtr - ptr;
}
