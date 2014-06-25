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

    /**
     * used for ignotium and whitespace tags who don't have any qualifiers.
     */
    static vector<string> emptyQualifiers;

    MorphInterpretation::MorphInterpretation(
            int startNode,
            int endNode,
            const string& orth,
            const string& lemma,
            int tagnum,
            int namenum,
            const vector<string>* qualifiers,
            const Tagset<string>* tagset)
    : startNode(startNode),
    endNode(endNode),
    orth(orth),
    lemma(lemma),
    tagnum(tagnum),
    namenum(namenum),
    qualifiers(qualifiers),
    tagset(tagset) {
    }

    MorphInterpretation::MorphInterpretation()
    : startNode(),
    endNode(),
    orth(),
    lemma(),
    tagnum(),
    namenum(),
    qualifiers(&emptyQualifiers),
    tagset(NULL) {

    }

    MorphInterpretation MorphInterpretation::createIgn(int startNode, int endNode, const std::string& orth, const Tagset<string>& tagset) {
        MorphInterpretation mi(startNode, endNode, orth, orth, 0, 0, &emptyQualifiers, &tagset);
        return mi;
    }
    
    MorphInterpretation MorphInterpretation::createWhitespace(int startNode, int endNode, const std::string& orth, const Tagset<string>& tagset) {
        MorphInterpretation mi(startNode, endNode, orth, orth, 1, 0, &emptyQualifiers, &tagset);
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

    static inline string getQualifiersStr(const MorphInterpretation& mi) {
        string res;
        for (unsigned int i = 0; i < mi.getQualifiers().size(); i++) {
            res += mi.getQualifiers()[i];
            if (i + 1 < mi.getQualifiers().size()) {
                res += "|";
            }
        }
        return res;
    }

    std::string MorphInterpretation::toString(bool includeNodeNumbers) const {
        std::stringstream res;
        if (includeNodeNumbers) {
            res << startNode << "," << endNode << ",";
        }
        res << orth << ",";

        res << lemma;
        res << ",";

        res << getTag();
        if (!getName().empty()) {
            res << "," << getName();
        }
        if (!getQualifiers().empty()) {
            res << "," << getQualifiersStr(*this);
        }
        return res.str();
    }

}
