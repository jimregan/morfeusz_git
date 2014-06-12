/* 
 * File:   MorphInterpretation.cpp
 * Author: mlenart
 * 
 * Created on November 14, 2013, 11:47 AM
 */

#include <string>
#include <sstream>
#include "MorphInterpretation.hpp"
#include "const.hpp"

using namespace std;

MorphInterpretation::MorphInterpretation(
        int startNode,
        int endNode,
        const string& orth,
        const string& lemma,
        //        const string& homonymId,
        int tagnum,
        int namenum,
        int qualifiersNum,
        const Environment& env)
: startNode(startNode),
endNode(endNode),
orth(orth),
lemma(lemma),
//homonymId(homonymId),
tagnum(tagnum),
namenum(namenum),
tag(env.getTagset().getTag(tagnum, env.getCharsetConverter())),
name(env.getTagset().getName(namenum, env.getCharsetConverter())),
qualifiers(&env.getQualifiersHelper().getQualifiers(qualifiersNum)) {


}

static const vector<std::string> emptyQualifiers;

MorphInterpretation::MorphInterpretation() 
: startNode(),
endNode(),
orth(),
lemma(),
//homonymId(homonymId),
tagnum(),
namenum(),
tag(),
name(),
qualifiers(&emptyQualifiers){
    
}

MorphInterpretation::MorphInterpretation(
        int startNode,
        const std::string& orth,
        const Environment& env)
: startNode(startNode),
endNode(startNode + 1),
orth(orth),
lemma(orth),
//homonymId(""),
tagnum(0),
namenum(0),
//        qualifiersNum(0),
tag(env.getTagset().getTag(0, env.getCharsetConverter())),
name(env.getTagset().getName(0, env.getCharsetConverter())),
qualifiers(&emptyQualifiers) {

}

MorphInterpretation MorphInterpretation::createIgn(int startNode, const std::string& orth, const Environment& env) {
    return MorphInterpretation(startNode, orth, env);
}

int MorphInterpretation::getStartNode() const {
    return this->startNode;
}

int MorphInterpretation::getEndNode() const {
    return this->endNode;
}

const std::string& MorphInterpretation::getOrth() const {
    return this->orth;
}

const std::string& MorphInterpretation::getLemma() const {
    return this->lemma;
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

//const std::string& MorphInterpretation::getHomonymId() const {
//    return this->homonymId;
//}

int MorphInterpretation::getTagnum() const {
    return this->tagnum;
}

int MorphInterpretation::getNamenum() const {
    return this->namenum;
}

const std::string& MorphInterpretation::getTag() const {
    return this->tag;
}

const std::string& MorphInterpretation::getName() const {
    return this->name;
}

const vector<string>& MorphInterpretation::getQualifiers() const {
    return *this->qualifiers;
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
//    if (!this->homonymId.empty()) {
//        res << ":" << homonymId;
//    }
    res << ",";

    res << tag;
    if (!name.empty()) {
        res << "," << name;
    }
    if (!qualifiers->empty()) {
        res << "," << getQualifiersStr(*this);
    }
    return res.str();
}
