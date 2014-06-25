/* 
 * File:   InterpretedChunksDecoder4Generator.cpp
 * Author: mlenart
 * 
 * Created on 15 maj 2014, 15:28
 */

#include "InterpretedChunksDecoder4Generator.hpp"
#include <string>
#include <vector>

using namespace std;

namespace morfeusz {

InterpretedChunksDecoder4Generator::InterpretedChunksDecoder4Generator(const Environment& env) : InterpretedChunksDecoder(env) {
}

void InterpretedChunksDecoder4Generator::decode(
        unsigned int startNode,
        unsigned int endNode,
        const InterpretedChunk& interpretedChunk,
        std::vector<MorphInterpretation>& out) const {
    string orthPrefix;
    string lemma;
//    convertPrefixes(interpretedChunk, orthPrefix, lemma);
    //        lemma += env.getCharsetConverter().toString(interpretedChunk.originalCodepoints);
    lemma.insert(lemma.end(), interpretedChunk.textStartPtr, interpretedChunk.textEndPtr);
    const unsigned char* currPtr = getInterpretationsPtr(interpretedChunk.interpsGroupPtr);
    while (currPtr < interpretedChunk.interpsEndPtr) {
        MorphInterpretation mi = this->decodeMorphInterpretation(startNode, endNode, orthPrefix, lemma, interpretedChunk, currPtr);
        //                        cerr << mi.toString(false) << endl;
        //            cerr << "required='" << interpretedChunk.requiredHomonymId << "' morphInterp='" << mi.getHomonymId() << "'" << endl;
        if (interpretedChunk.requiredHomonymId.empty() || mi.hasHomonym(interpretedChunk.requiredHomonymId)) {
            out.push_back(mi);
        }
    }
}

//void InterpretedChunksDecoder4Generator::convertPrefixes(const InterpretedChunk& interpretedChunk, std::string& orthPrefix, std::string& lemma) const {
//    for (unsigned int i = 0; i < interpretedChunk.prefixChunks.size(); i++) {
//        const InterpretedChunk& prefixChunk = interpretedChunk.prefixChunks[i];
////        lemma.insert(lemma.end(), prefixChunk.textStartPtr, prefixChunk.textEndPtr);
//        const unsigned char* ptr = getInterpretationsPtr(interpretedChunk.interpsGroupPtr);
//        MorphInterpretation mi = this->decodeMorphInterpretation(0, 0, orthPrefix, string(""), prefixChunk, ptr);
////        orthPrefix += mi.getOrth();
//    }
//}

MorphInterpretation InterpretedChunksDecoder4Generator::decodeMorphInterpretation(
        unsigned int startNode, unsigned int endNode,
        const string& orthPrefix,
        const string& lemma,
        const InterpretedChunk& chunk,
        const unsigned char*& ptr) const {
    string orth = orthPrefix;
    EncodedInterpretation ei = this->deserializeInterp(ptr);
    codepoints.resize(0);
    const char* currPtr = chunk.textStartPtr;
    while (currPtr != chunk.textEndPtr) {
        uint32_t cp = env.getCharsetConverter().next(currPtr, chunk.textEndPtr);
        codepoints.push_back(cp);
    }
    this->decodeForm(codepoints, ei.value, orth);
    MorphInterpretation res(
            startNode, endNode,
            orth, ei.homonymId.empty() ? lemma : (lemma + HOMONYM_SEPARATOR + ei.homonymId),
            //                ei.homonymId,
            ei.tag,
            ei.nameClassifier,
            &env.getQualifiersHelper().getQualifiers(ei.qualifiers),
            &env.getTagset());
    return res;
}

void InterpretedChunksDecoder4Generator::decodeForm(
        const vector<uint32_t>& lemma,
        const EncodedForm& orth,
        string& res) const {
    res += orth.prefixToAdd;
    for (unsigned int i = 0; i < lemma.size() - orth.suffixToCut; i++) {
        env.getCharsetConverter().append(lemma[i], res);
    }
    const char* suffixPtr = orth.suffixToAdd.c_str();
    const char* suffixEnd = suffixPtr + orth.suffixToAdd.length();
    while (suffixPtr != suffixEnd) {
        uint32_t cp = UTF8CharsetConverter::getInstance().next(suffixPtr, suffixEnd);
        env.getCharsetConverter().append(cp, res);
    }
}

EncodedInterpretation InterpretedChunksDecoder4Generator::deserializeInterp(const unsigned char*& ptr) const {
    EncodedInterpretation interp;
    interp.homonymId = readString(ptr);
    interp.value.prefixToAdd = readString(ptr);
    interp.value.suffixToCut = readInt8(ptr);
    interp.value.suffixToAdd = readString(ptr);
    interp.tag = readInt16(ptr);
    interp.nameClassifier = readInt8(ptr);
    interp.qualifiers = readInt16(ptr);
    return interp;
}

}
