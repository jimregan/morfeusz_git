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
#include "deserializationUtils.hpp"

//const uint8_t LEMMA_ONLY_LOWER = 0;
//const uint8_t LEMMA_UPPER_PREFIX = 1;
//const uint8_t LEMMA_MIXED_CASE = 2;
//const unsigned int MAX_WORD_SIZE = 256;

MorphDeserializer::MorphDeserializer() {
}

MorphDeserializer::~MorphDeserializer() {
}

long MorphDeserializer::deserialize(const unsigned char* ptr, vector<InterpsGroup>& interps) const {
    const unsigned char* currPtr = ptr;
    uint8_t interpTypesNum = readInt8(currPtr);
    interps.clear();
    interps.reserve(interpTypesNum);
    for (unsigned int i = 0; i < interpTypesNum; i++) {
        InterpsGroup ig;
        ig.type = readInt8(currPtr);
        ig.size = readInt16(currPtr);
        ig.ptr = currPtr;
        currPtr += ig.size;
        interps.push_back(ig);
    }
    return currPtr - ptr;
}
