/* 
 * File:   MorphInterpretation.cpp
 * Author: mlenart
 * 
 * Created on November 14, 2013, 11:47 AM
 */

#include <string>
#include "MorphInterpretation.hpp"
#include "EncodedInterpretation.hpp"

using namespace std;

MorphInterpretation::MorphInterpretation(
        int startNode,
        int endNode,
        const string& orth,
        const string& lemma,
        int tagnum,
        int namenum,
        const Tagset& tagset)
: startNode(startNode), 
        endNode(endNode), 
        orth(orth),
        lemma(lemma),
        tagnum(tagnum),
        namenum(namenum),
        tag(tagset.getTag(tagnum)),
        name(tagset.getName(namenum)) {

}

MorphInterpretation::MorphInterpretation(
        int startNode,
        const std::string& orth,
        const Tagset& tagset)
: startNode(startNode), 
        endNode(startNode + 1), 
        orth(orth),
        lemma(orth),
        tagnum(0),
        namenum(0),
        tag(tagset.getTag(0)),
        name(tagset.getName(0)) {

}

MorphInterpretation MorphInterpretation::createIgn(int startNode, const std::string& orth, const Tagset& tagset) {
    return MorphInterpretation(startNode, orth, tagset);
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

