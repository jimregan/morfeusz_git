/* 
 * File:   MorphInterpretation.cpp
 * Author: mlenart
 * 
 * Created on November 14, 2013, 11:47 AM
 */

#include <string>
#include <sstream>
#include "morfeusz2.h"
#include "Environment.hpp"
#include "const.hpp"

using namespace std;

namespace morfeusz {

    MorphInterpretation MorphInterpretation::createIgn(
            int startNode, int endNode,
            const std::string& orth,
            const std::string& lemma) {
        MorphInterpretation mi;
        mi.startNode = startNode;
        mi.endNode = endNode;
        mi.orth = orth;
        mi.lemma = lemma;
        mi.tagId = 0;
        mi.nameId = 0;
        mi.labelsId = 0;
        return mi;
    }

    /**
     * Creates new instance with "sp" tag (meaning: "this is a sequence of whitespaces")
     */
    MorphInterpretation MorphInterpretation::createWhitespace(int startNode, int endNode, const std::string& orth) {
        MorphInterpretation mi;
        mi.startNode = startNode;
        mi.endNode = endNode;
        mi.orth = orth;
        mi.lemma = orth;
        mi.tagId = 1;
        mi.nameId = 0;
        mi.labelsId = 0;
        return mi;
    }

    static inline bool hasEnding(const string &fullString, const string &ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
        }
        else {
            return false;
        }
    }

    bool MorphInterpretation::hasHomonym(const string& homonymId) const {
        size_t homonymSeparatorIdx = this->lemma.length() - homonymId.length() - 1;
        return homonymSeparatorIdx > 0
                && this->lemma[homonymSeparatorIdx] == HOMONYM_SEPARATOR
                && hasEnding(this->lemma, homonymId);
    }

    std::string MorphInterpretation::toString(bool includeNodeNumbers) const {
        std::stringstream res;
        if (includeNodeNumbers) {
            res << startNode << "," << endNode << ",";
        }
        res << orth << ",";

        res << lemma;
        res << ",";

        //        res << getTag();
        //        if (!getName().empty()) {
        //            res << "," << getName();
        //        }
        //        if (!getQualifiers().empty()) {
        //            res << "," << getQualifiersStr(*this);
        //        }
        return res.str();
    }

}
