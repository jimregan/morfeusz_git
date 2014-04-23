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
#include "CasePatternHelper.hpp"
#include "compressionByteUtils.hpp"

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

    bool checkInterpsGroupOrthCasePatterns(
            const std::vector<uint32_t>& lowercaseCodepoints,
            const std::vector<uint32_t>& originalCodepoints,
            const InterpsGroup& ig) const {
        const unsigned char* currPtr = ig.ptr;
        unsigned char compressionByte = *currPtr++;
        if (!this->caseSensitive) {
            return true;
        }
        else if (isOrthOnlyLower(compressionByte)) {
            return true;
        }
        else if (isOrthOnlyTitle(compressionByte)) {
            return lowercaseCodepoints[0] != originalCodepoints[0];
        } else {
            unsigned char casePatternsNum = *currPtr++;
            if (casePatternsNum == 0) {
                return true;
            } else {
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
private:
    bool caseSensitive;

    static const uint8_t LEMMA_ONLY_LOWER = 0;
    static const uint8_t LEMMA_UPPER_PREFIX = 1;
    static const uint8_t LEMMA_MIXED_CASE = 2;
};

#endif	/* CASEPATTERNHELPER_HPP */

