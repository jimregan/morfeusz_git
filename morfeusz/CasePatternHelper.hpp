/* 
 * File:   CasePatternHelper.hpp
 * Author: lennyn
 *
 * Created on April 4, 2014, 12:11 PM
 */

#ifndef CASEPATTERNHELPER_HPP
#define	CASEPATTERNHELPER_HPP

#include <vector>
#include "InterpsGroup.hpp"

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
    
    bool checkInterpsGroupCasePatterns(
        const std::vector<uint32_t>& lowercaseCodepoints, 
        const std::vector<uint32_t>& originalCodepoints,
        const InterpsGroup& ig) const {
        const unsigned char* currPtr = ig.ptr;
        unsigned char casePatternsNum = *currPtr++;
        if (casePatternsNum == 0) {
            return true;
        }
        else {
            for (unsigned int i = 0; i < casePatternsNum; i++) {
                if (checkCasePattern(
                        lowercaseCodepoints, 
                        originalCodepoints, 
                        deserializeOneCasePattern(currPtr))) {
                    return true;
                }
            }
            return false;
        }
    }
    
    const unsigned char* getInterpretationsPtr(const InterpsGroup& ig) const {
        const unsigned char* currPtr = ig.ptr;
        unsigned char casePatternsNum = *currPtr++;
        for (unsigned int i = 0; i < casePatternsNum; i++) {
            deserializeOneCasePattern(currPtr);
        }
        return currPtr;
    }
    
    std::vector<bool> deserializeOneCasePattern(const unsigned char*& ptr) const {
        std::vector<bool> res;
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
        return res;
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

//    void skipCasePattern(const unsigned char*& ptr) const {
//        vector<bool> _dupa;
//        deserializeCasePattern(ptr, _dupa);
//    }
private:
    bool caseSensitive;
    
};

#endif	/* CASEPATTERNHELPER_HPP */

