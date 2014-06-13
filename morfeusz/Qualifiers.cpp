/* 
 * File:   Qualifiers.cpp
 * Author: lennyn
 * 
 * Created on April 4, 2014, 6:19 PM
 */

#include <iostream>
#include "Qualifiers.hpp"
#include "deserialization/deserializationUtils.hpp"
#include "fsa/const.hpp"

using namespace std;

namespace morfeusz {

Qualifiers::Qualifiers(const unsigned char* ptr):
qualifiers() {
    uint32_t fsaSize = readInt32Const(ptr + FSA_DATA_SIZE_OFFSET);
    const unsigned char* currPtr = ptr + FSA_DATA_OFFSET + fsaSize + 4;
    vector<string> _dupa;
    readTags(currPtr, _dupa);
    _dupa.clear();
    readTags(currPtr, _dupa);
    uint16_t allCombinationsSize = readInt16(currPtr);
    this->qualifiers.reserve(allCombinationsSize);
    for (unsigned int i = 0; i < allCombinationsSize; i++) {
        unsigned char qualsNum = readInt8(currPtr);
        vector<string> quals;
        for (unsigned int j = 0; j < qualsNum; j++) {
            quals.push_back(readString(currPtr));
        }
        this->qualifiers.push_back(quals);
    }
}

const vector<string>& Qualifiers::getQualifiers(int n) const {
    return this->qualifiers.at(n);
}

Qualifiers::~Qualifiers() {
}

}
