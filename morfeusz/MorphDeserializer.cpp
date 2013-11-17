/* 
 * File:   MorphDeserializer.cpp
 * Author: mlenart
 * 
 * Created on 12 listopad 2013, 15:31
 */

#include <map>
#include "MorphDeserializer.hpp"
#include "EncodedInterpretation.hpp"
#include "InterpsGroup.hpp"

MorphDeserializer::MorphDeserializer() {
}

MorphDeserializer::MorphDeserializer(const MorphDeserializer& orig) {
}

MorphDeserializer::~MorphDeserializer() {
}

static void deserializeLemma(const unsigned char*& ptr, EncodedLemma& lemma) {
    // XXX uważać na poprawność danych
    lemma.suffixToCut = *ptr;
    ptr++;
    lemma.suffixToAdd = (const char*) ptr;
    ptr += strlen((const char*) ptr) + 1;
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
    for (auto& kv: results) {
        interps.push_back(kv.second);
    }
    return currPtr - ptr;
}

//static void deserializeLemma(const unsigned char*& ptr, EncodedLemma& lemma) {
//    // XXX uważać na poprawność danych
//    lemma.suffixToCut = *ptr;
//    ptr++;
//    lemma.suffixToAdd = (const char*) ptr;
//    ptr += strlen((const char*) ptr) + 1;
//}
//
//static void deserializeInterp(const unsigned char*& ptr, EncodedInterpretation& interp) {
//    interp.type = *ptr;
//    ptr++;
//    deserializeLemma(ptr, interp.lemma);
//    interp.tag = ntohs(*(reinterpret_cast<const uint16_t*>(ptr)));
//    ptr += 2;
//    interp.nameClassifier = *ptr;
//    ptr++;
//}
//
//long MorphDeserializer::deserialize(const unsigned char* ptr, vector<EncodedInterpretation>& interps) const {
//    const unsigned char* currPtr = ptr;
//    uint8_t interpsNum = *ptr;
//    interps.clear();
//    interps.reserve(interpsNum);
//    currPtr++;
//    for (unsigned int i = 0; i < interpsNum; ++i) {
//        EncodedInterpretation interp;
//        deserializeInterp(currPtr, interp);
//        interps.push_back(interp);
//    }
//    return currPtr - ptr;
//}

