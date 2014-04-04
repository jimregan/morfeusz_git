/* 
 * File:   Qualifiers.cpp
 * Author: lennyn
 * 
 * Created on April 4, 2014, 6:19 PM
 */

#include <iostream>
#include "Qualifiers.hpp"
#include "deserializationUtils.hpp"
#include "fsa/const.hpp"

using namespace std;

Qualifiers::Qualifiers(const unsigned char* ptr) {
    uint32_t fsaSize = htonl(*reinterpret_cast<const uint32_t*>(ptr + FSA_DATA_SIZE_OFFSET));
    const unsigned char* currPtr = ptr + FSA_DATA_OFFSET + fsaSize + 4;
    vector<string> _dupa;
    readTags(currPtr, _dupa);
    _dupa.clear();
    readTags(currPtr, _dupa);
    
    uint16_t allCombinationsSize = readInt16(currPtr);
    this->qualifiers.reserve(allCombinationsSize);
    for (unsigned int i = 0; i < allCombinationsSize; i++) {
        unsigned char qualsNum = *currPtr++;
        vector<string> quals;
        for (unsigned int j = 0; j < qualsNum; j++) {
            quals.push_back(readString(currPtr));
        }
        this->qualifiers.push_back(quals);
    }
}

vector<string> Qualifiers::getQualifiers(uint16_t n) const {
    return this->qualifiers[n];
}

Qualifiers::~Qualifiers() {
}
