/* 
 * File:   CasePatternHelper.hpp
 * Author: lennyn
 *
 * Created on April 4, 2014, 12:11 PM
 */

#ifndef CASEPATTERNHELPER_HPP
#define	CASEPATTERNHELPER_HPP

#include <vector>
#include "InterpretedChunk.hpp"

const uint8_t LEMMA_ONLY_LOWER = 0;
const uint8_t LEMMA_UPPER_PREFIX = 1;
const uint8_t LEMMA_MIXED_CASE = 2;

class CasePatternHelper {
public:

    CasePatternHelper() : caseSensitive(false) {

    }

    void setCaseSensitive(bool caseSensitive) {
        this->caseSensitive = caseSensitive;
    }

    bool checkCasePattern(
        const std::vector<uint32_t>& lowercaseCodepoints, 
        const std::vector<uint32_t>& originalCodepoints, 
        const std::vector<bool>& casePattern) const {
        if (this->caseSensitive) {
            for (unsigned int i = 0; i < casePattern.size(); i++) {
                if (casePattern[i] && lowercaseCodepoints[i] == originalCodepoints[i]) {
                    return false;
                }
            }
        }
        return true;
    }

//    bool checkCasePattern(const std::vector<InterpretedChunk>& chunks) const {
//        if (this->caseSensitive) {
//            for (unsigned int i = 0; i < chunks.size(); i++) {
//                const InterpretedChunk& ic = chunks[i];
//                const unsigned char* casePatternPtr = ic.interpsGroup.ptr;
//                std::vector<bool> casePattern;
//                deserializeCasePattern(casePatternPtr, casePattern);
//                if (!checkCasePattern(ic, casePattern)) {
//                    return false;
//                }
//            }
//        }
//        return true;
//    }

    void skipCasePattern(const unsigned char*& ptr) const {
        vector<bool> _dupa;
        deserializeCasePattern(ptr, _dupa);
    }

    void deserializeCasePattern(const unsigned char*& ptr, std::vector<bool>& res) const {
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
                    //                lemma.casePattern[i] = true;
                    res.push_back(true);
                }
                //            lemma.casePattern.resize(prefixLength, true);
                break;
            case LEMMA_MIXED_CASE:
                patternLength = *ptr;
                ptr++;
                for (unsigned int i = 0; i < patternLength; i++) {
                    uint8_t idx = *ptr;
                    ptr++;
                    //                lemma.casePattern[idx] = true;
                    res.resize(idx + 1, false);
                    res[idx] = true;
                }
                break;
        }
    }
private:
    bool caseSensitive;
};

#endif	/* CASEPATTERNHELPER_HPP */

