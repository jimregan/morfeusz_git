/* 
 * File:   MorphDeserializer.cpp
 * Author: mlenart
 * 
 * Created on 12 listopad 2013, 15:31
 */

#include "MorphDeserializer.hpp"

MorphDeserializer::MorphDeserializer() {
}

MorphDeserializer::MorphDeserializer(const MorphDeserializer& orig) {
}

MorphDeserializer::~MorphDeserializer() {
}

static void deserializeLemma(const unsigned char*& ptr, Lemma& lemma) {
    // XXX uważać na poprawność danych
    lemma.suffixToCut = *ptr;
    ptr++;
    lemma.suffixToAdd = (const char*) ptr;
    ptr += strlen((const char*) ptr) + 1;
}

static void deserializeInterp(const unsigned char*& ptr, Interpretation& interp) {
    deserializeLemma(ptr, interp.lemma);
    interp.tag = ntohs(*(reinterpret_cast<const uint16_t*>(ptr)));
    ptr += 2;
    interp.nameClassifier = *ptr;
    ptr++;
}

long MorphDeserializer::deserialize(const unsigned char* ptr, vector<Interpretation>& interps) const {
    const unsigned char* currPtr = ptr;
    uint8_t interpsNum = *ptr;
    interps.clear();
    interps.reserve(interpsNum);
    currPtr++;
    for (unsigned int i = 0; i < interpsNum; i++) {
        Interpretation interp;
        deserializeInterp(currPtr, interp);
        interps.push_back(interp);
    }
    return currPtr - ptr;
}

