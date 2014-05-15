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
#include "Environment.hpp"

class Environment;

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
            const Environment& env,
            const char* orthStart,
            const char* orthEnd,
            const InterpsGroup& ig) const;

    static std::vector<bool> deserializeOneCasePattern(const unsigned char*& ptr);
private:
    bool caseSensitive;
    
    mutable vector<uint32_t> orthCodepoints;
    mutable vector<uint32_t> normalizedCodepoints;

    static const uint8_t LEMMA_ONLY_LOWER = 0;
    static const uint8_t LEMMA_UPPER_PREFIX = 1;
    static const uint8_t LEMMA_MIXED_CASE = 2;
};

#endif	/* CASEPATTERNHELPER_HPP */

