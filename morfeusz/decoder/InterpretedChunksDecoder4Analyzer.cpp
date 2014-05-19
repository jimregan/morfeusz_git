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
    string lemma4Prefixes;
    if (tryToGetLemma4Prefixes(interpretedChunk, lemma4Prefixes)) {
        orth.insert(orth.end(), interpretedChunk.textStartPtr, interpretedChunk.textEndPtr);
        const unsigned char* currPtr = interpretedChunk.interpsPtr;
        while (currPtr < interpretedChunk.interpsEndPtr) {
            DecodeMorphInterpParams params = {startNode, endNode, orth, lemma4Prefixes, interpretedChunk};
            this->decodeMorphInterpretation(params, currPtr, out);
        }
    }
}

void InterpretedChunksDecoder4Analyzer::decodeLemma(
        const EncodedForm& lemma,
        int nonPrefixCodepointsNum,
        bool forPrefix,
        string& res) const {
    assert(nonPrefixCodepointsNum > orth.size());
    unsigned int prefixSegmentsOrthLength = forPrefix
        ? 0
        : (unsigned int) normalizedCodepoints.size() - nonPrefixCodepointsNum;
    size_t endIdx = forPrefix
        ? normalizedCodepoints.size()
        : normalizedCodepoints.size() - lemma.suffixToCut;
    for (unsigned int i = prefixSegmentsOrthLength + lemma.prefixToCut; i < endIdx; i++) {
        uint32_t cp =
                (i < lemma.casePattern.size() && lemma.casePattern[i])
                ? env.getCaseConverter().toTitle(normalizedCodepoints[i])
                : normalizedCodepoints[i];
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

void InterpretedChunksDecoder4Analyzer::decodeEncodedForm(const unsigned char*& ptr, unsigned char compressionByte, EncodedForm& encodedForm) const {
    encodedForm.prefixToCut = hasCompressedPrefixCut(compressionByte)
            ? getPrefixCutLength(compressionByte)
            : readInt8(ptr);
    encodedForm.suffixToCut = readInt8(ptr);
    encodedForm.suffixToAdd = readString(ptr);
    assert(encodedForm.casePattern.size() == 0);
    if (isLemmaOnlyLower(compressionByte)) {
    }
    else if (isLemmaOnlyTitle(compressionByte)) {
        encodedForm.casePattern.push_back(true);
    }
    else {
        encodedForm.casePattern = env.getCasePatternHelper().deserializeOneCasePattern(ptr);
    }
}

EncodedInterpretation InterpretedChunksDecoder4Analyzer::decodeEncodedInterp(const unsigned char*& ptr, unsigned char compressionByte) const {
    EncodedInterpretation interp;
    if (isOrthOnlyLower(compressionByte)) {
    }
    else if (isOrthOnlyTitle(compressionByte)) {
        interp.orthCasePattern.push_back(true);
    }
    else {
        interp.orthCasePattern = this->env.getCasePatternHelper().deserializeOneCasePattern(ptr);
    }
    decodeEncodedForm(ptr, compressionByte, interp.value);
    interp.tag = readInt16(ptr);
    interp.nameClassifier = *ptr++;
    interp.qualifiers = readInt16(ptr);
    return interp;
}

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
    EncodedInterpretation ei = this->decodeEncodedInterp(ptr, *params.chunk.interpsGroupPtr);
    if (env.getCasePatternHelper().checkCasePattern(normalizedCodepoints, orthCodepoints, ei.orthCasePattern)) {
        string lemma(params.lemma4Prefixes);
        lemma.reserve(lemma.size() + 2 * normalizedCodepoints.size());
        this->decodeLemma(ei.value, params.chunk.codepointsNum, false, lemma);
        out.push_back(MorphInterpretation(
                params.startNode, params.endNode,
                params.orth, lemma,
                ei.tag,
                ei.nameClassifier,
                ei.qualifiers,
                env));
    }
}

bool InterpretedChunksDecoder4Analyzer::tryToGetLemma4Prefixes(
        const InterpretedChunk& interpretedChunk,
        string& lemma4Prefixes) const {
    for (unsigned int i = 0; i < interpretedChunk.prefixChunks.size(); i++) {
        const InterpretedChunk& prefixChunk = interpretedChunk.prefixChunks[i];
        if (!tryToGetLemma4OnePrefix(prefixChunk, lemma4Prefixes)) {
            return false;
        }
    }
    return true;
}

bool InterpretedChunksDecoder4Analyzer::tryToGetLemma4OnePrefix(const InterpretedChunk& prefixChunk, std::string& lemma4Prefixes) const {
    orthCodepoints.clear();
    normalizedCodepoints.clear();
    const char* currTextPtr = prefixChunk.textStartPtr;
    while (currTextPtr != prefixChunk.textEndPtr) {
        uint32_t cp = env.getCharsetConverter().next(currTextPtr, prefixChunk.textEndPtr);
        orthCodepoints.push_back(cp);
        normalizedCodepoints.push_back(env.getCaseConverter().toLower(cp));
    }
    const unsigned char* currPtr = prefixChunk.interpsPtr;
    EncodedInterpretation ei = this->decodeEncodedInterp(currPtr, *prefixChunk.interpsGroupPtr);
    if (env.getCasePatternHelper().checkCasePattern(normalizedCodepoints, orthCodepoints, ei.orthCasePattern)) {
        this->decodeLemma(ei.value, prefixChunk.codepointsNum, true, lemma4Prefixes);
        return true;
    }
    else {
        return false;
    }
}
