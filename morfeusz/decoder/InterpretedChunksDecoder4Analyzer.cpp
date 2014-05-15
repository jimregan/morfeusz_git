/* 
 * File:   InterpretedChunksDecoder4Analyzer.cpp
 * Author: mlenart
 * 
 * Created on 15 maj 2014, 15:28
 */

#include "InterpretedChunksDecoder4Analyzer.hpp"
#include <string>

using namespace std;

InterpretedChunksDecoder4Analyzer::InterpretedChunksDecoder4Analyzer(const Environment& env) : InterpretedChunksDecoder(env) {
}

void InterpretedChunksDecoder4Analyzer::decode(
        unsigned int startNode,
        unsigned int endNode,
        const InterpretedChunk& interpretedChunk,
        std::vector<MorphInterpretation>& out) const {
    string orth;
    string lemmaPrefix;
    if (convertPrefixes(interpretedChunk, orth, lemmaPrefix)) {
        //            orth += this->env.getCharsetConverter().toString(interpretedChunk.originalCodepoints);
        orth.insert(orth.end(), interpretedChunk.textStartPtr, interpretedChunk.textEndPtr);
        const unsigned char* currPtr = interpretedChunk.interpsPtr;
        while (currPtr < interpretedChunk.interpsEndPtr) {
            DecodeMorphInterpParams params = {startNode, endNode, orth, lemmaPrefix, interpretedChunk, false};
            this->decodeMorphInterpretation(params, currPtr, out);
//            this->decodeMorphInterpretation(startNode, endNode, orth, lemmaPrefix, interpretedChunk, false, currPtr, out);
        }
    }
}

void InterpretedChunksDecoder4Analyzer::decodeLemma(
        const vector<uint32_t>& orth,
        const EncodedForm& lemma,
        bool forPrefix,
        string& res) const {
    for (unsigned int i = lemma.prefixToCut; i < orth.size() - lemma.suffixToCut; i++) {
        uint32_t cp =
                (i < lemma.casePattern.size() && lemma.casePattern[i])
                ? env.getCaseConverter().toTitle(orth[i])
                : orth[i];
        env.getCharsetConverter().append(cp, res);
    }
    if (!forPrefix) {
        const char* suffixPtr = lemma.suffixToAdd.c_str();
        const char* suffixEnd = suffixPtr + lemma.suffixToAdd.length();
        while (suffixPtr != suffixEnd) {
            uint32_t cp = UTF8CharsetConverter::getInstance().next(suffixPtr, suffixEnd);
            env.getCharsetConverter().append(cp, res);
        }
    }
}

void InterpretedChunksDecoder4Analyzer::deserializeEncodedForm(const unsigned char*& ptr, unsigned char compressionByte, EncodedForm& encodedForm) const {
    encodedForm.prefixToCut = hasCompressedPrefixCut(compressionByte)
            ? getPrefixCutLength(compressionByte)
            : readInt8(ptr);
    encodedForm.suffixToCut = readInt8(ptr);
    encodedForm.suffixToAdd = readString(ptr);
    assert(encodedForm.casePattern.size() == 0);
    if (isLemmaOnlyLower(compressionByte)) {
//        encodedForm.casePattern = std::vector<bool>();
    }
    else if (isLemmaOnlyTitle(compressionByte)) {
//        encodedForm.casePattern = std::vector<bool>();
        encodedForm.casePattern.push_back(true);
    }
    else {
        encodedForm.casePattern = env.getCasePatternHelper().deserializeOneCasePattern(ptr);
    }
}

EncodedInterpretation InterpretedChunksDecoder4Analyzer::deserializeEncodedInterp(const unsigned char*& ptr, unsigned char compressionByte) const {
    EncodedInterpretation interp;
    if (isOrthOnlyLower(compressionByte)) {
    }
    else if (isOrthOnlyTitle(compressionByte)) {
        interp.orthCasePattern.push_back(true);
    }
    else {
        interp.orthCasePattern = this->env.getCasePatternHelper().deserializeOneCasePattern(ptr);
    }
    deserializeEncodedForm(ptr, compressionByte, interp.value);
    interp.tag = readInt16(ptr);
    interp.nameClassifier = *ptr++;
    interp.qualifiers = readInt16(ptr);
    return interp;
}

//void InterpretedChunksDecoder4Analyzer::decodeMorphInterpretation(
//        unsigned int startNode, unsigned int endNode,
//        const string& orth,
//        const string& lemmaPrefix,
//        const InterpretedChunk& chunk,
//        bool forPrefix,
//        const unsigned char*& ptr,
//        std::vector<MorphInterpretation>& out) const {
//    orthCodepoints.clear();
//    normalizedCodepoints.clear();
//    const char* currPtr = chunk.textStartPtr;
//    while (currPtr != chunk.textEndPtr) {
//        uint32_t cp = env.getCharsetConverter().next(currPtr, chunk.textEndPtr);
//        orthCodepoints.push_back(cp);
//        normalizedCodepoints.push_back(env.getCaseConverter().toLower(cp));
//    }
//    EncodedInterpretation ei = this->deserializeEncodedInterp(ptr, *chunk.interpsGroupPtr);
//    if (env.getCasePatternHelper().checkCasePattern(normalizedCodepoints, orthCodepoints, ei.orthCasePattern)) {
//        string lemma(lemmaPrefix);
//        lemma.reserve(lemma.size() + 2 * normalizedCodepoints.size());
//        this->decodeLemma(normalizedCodepoints, ei.value, forPrefix, lemma);
//        out.push_back(MorphInterpretation(
//                startNode, endNode,
//                orth, lemma,
//                ei.tag,
//                ei.nameClassifier,
//                ei.qualifiers,
//                env));
//    }
//}

void InterpretedChunksDecoder4Analyzer::decodeMorphInterpretation(
        const DecodeMorphInterpParams& params, 
        const unsigned char*& ptr, 
        std::vector<MorphInterpretation>& out) const {
    orthCodepoints.clear();
    normalizedCodepoints.clear();
    const char* currPtr = params.chunk.textStartPtr;
    while (currPtr != params.chunk.textEndPtr) {
        uint32_t cp = env.getCharsetConverter().next(currPtr, params.chunk.textEndPtr);
        orthCodepoints.push_back(cp);
        normalizedCodepoints.push_back(env.getCaseConverter().toLower(cp));
    }
    EncodedInterpretation ei = this->deserializeEncodedInterp(ptr, *params.chunk.interpsGroupPtr);
    if (env.getCasePatternHelper().checkCasePattern(normalizedCodepoints, orthCodepoints, ei.orthCasePattern)) {
        string lemma(params.lemmaPrefix);
        lemma.reserve(lemma.size() + 2 * normalizedCodepoints.size());
        this->decodeLemma(normalizedCodepoints, ei.value, params.forPrefix, lemma);
        out.push_back(MorphInterpretation(
                params.startNode, params.endNode,
                params.orth, lemma,
                ei.tag,
                ei.nameClassifier,
                ei.qualifiers,
                env));
    }
}

bool InterpretedChunksDecoder4Analyzer::convertPrefixes(const InterpretedChunk& interpretedChunk, std::string& orth, std::string& lemmaPrefix) const {
    for (unsigned int i = 0; i < interpretedChunk.prefixChunks.size(); i++) {
        const InterpretedChunk& prefixChunk = interpretedChunk.prefixChunks[i];
        orth.insert(orth.end(), prefixChunk.textStartPtr, prefixChunk.textEndPtr);
        const unsigned char* ptr = prefixChunk.interpsPtr;
        std::vector<MorphInterpretation> mi;
        DecodeMorphInterpParams params = {0, 0, orth, string(""), prefixChunk, true};
        this->decodeMorphInterpretation(params, ptr, mi);
//        this->decodeMorphInterpretation(0, 0, orth, string(""), prefixChunk, true, ptr, mi);
        if (!mi.empty()) {
            lemmaPrefix += mi[0].getLemma();
        }
        else {
            return false;
        }
    }
    return true;
}
