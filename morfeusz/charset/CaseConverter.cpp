/* 
 * File:   CaseConverter.cpp
 * Author: lennyn
 * 
 * Created on November 22, 2013, 2:36 PM
 */

#include "CaseConverter.hpp"
#include "case_folding.hpp"

using namespace std;

//extern const unsigned int CASE_FOLDING_TABLE_SIZE;
//extern const unsigned int EXT_CASE_FOLDING_TABLE_SIZE;
//extern const uint32_t[] CASE_FOLDING_TABLE;
//extern const uint32_t[][2] EXT_CASE_FOLDING_TABLE;

map<uint32_t, uint32_t> initializeExtCaseMap() {
    map<uint32_t, uint32_t> res;
    for (unsigned int i = 0; i < EXT_CASE_FOLDING_TABLE_SIZE; i++) {
        uint32_t key = EXT_CASE_FOLDING_TABLE[i][0];
        uint32_t value = EXT_CASE_FOLDING_TABLE[i][1];
        res[key] = value;
    }
    return res;
}

CaseConverter::CaseConverter()
: extCaseMap(initializeExtCaseMap()) {
}

uint32_t CaseConverter::toLower(uint32_t codepoint) const {
    if (codepoint < CASE_FOLDING_TABLE_SIZE) {
        return CASE_FOLDING_TABLE[codepoint];
    }
    else if (this->extCaseMap.count(codepoint) != 0) {
       return this->extCaseMap.at(codepoint); 
    }
    else {
        return codepoint;
    }
}

