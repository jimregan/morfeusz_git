/* 
 * File:   InterpsGroupDecoder.hpp
 * Author: mlenart
 *
 * Created on November 22, 2013, 10:35 PM
 */

#ifndef INTERPSGROUPDECODER_HPP
#define	INTERPSGROUPDECODER_HPP

#include <string>
#include <vector>

#include "charset/CharsetConverter.hpp"
#include "EncodedInterpretation.hpp"
#include "InterpretedChunk.hpp"
#include "EncodedInterpretation.hpp"
#include "charset/CaseConverter.hpp"
#include "Environment.hpp"

const uint8_t LEMMA_ONLY_LOWER = 0;
const uint8_t LEMMA_UPPER_PREFIX = 1;
const uint8_t LEMMA_MIXED_CASE = 2;

class InterpretedChunksDecoder {
public:

    InterpretedChunksDecoder(const Environment& env)
    : env(env) {
    }

    virtual void decode(
            unsigned int startNode,
            unsigned int endNode,
            const InterpretedChunk& interpretedChunk,
            std::vector<MorphInterpretation>& out) const = 0;

    virtual ~InterpretedChunksDecoder() {
    }

protected:

    virtual void decodeForm(
            const std::vector<uint32_t>& orth,
            const EncodedForm& form,
            std::string& res) const = 0;

    const Environment& env;
};

class InterpretedChunksDecoder4Analyzer : public InterpretedChunksDecoder {
public:

    InterpretedChunksDecoder4Analyzer(const Environment& env) : InterpretedChunksDecoder(env) {
    }

    void decode(
            unsigned int startNode,
            unsigned int endNode,
            const InterpretedChunk& interpretedChunk,
            std::vector<MorphInterpretation>& out) const {
        string orth;
        string lemmaPrefix;
        convertPrefixes(interpretedChunk, orth, lemmaPrefix);
        orth += env.getCharsetConverter().toString(interpretedChunk.originalCodepoints);
        const unsigned char* currPtr = interpretedChunk.interpsGroup.ptr;
        while (currPtr - interpretedChunk.interpsGroup.ptr < interpretedChunk.interpsGroup.size) {
            out.push_back(this->decodeMorphInterpretation(startNode, endNode, orth, lemmaPrefix, interpretedChunk, currPtr));
        }
    }

protected:

    void decodeForm(
            const vector<uint32_t>& orth,
            const EncodedForm& lemma,
            string& res) const {
        for (unsigned int i = 0; i < orth.size() - lemma.suffixToCut; i++) {
            uint32_t cp =
                    (i < lemma.casePattern.size() && lemma.casePattern[i])
                    ? env.getCaseConverter().toTitle(orth[i])
                    : orth[i];
            env.getCharsetConverter().append(cp, res);
        }
        const char* suffixPtr = lemma.suffixToAdd.c_str();
        const char* suffixEnd = suffixPtr + lemma.suffixToAdd.length();
        while (suffixPtr != suffixEnd) {
            uint32_t cp = UTF8CharsetConverter().next(suffixPtr, suffixEnd);
            env.getCharsetConverter().append(cp, res);
        }
    }

private:

    void convertPrefixes(const InterpretedChunk& interpretedChunk, std::string& originalForm, std::string& decodedForm) const {
        for (unsigned int i = 0; i < interpretedChunk.prefixChunks.size(); i++) {
            const InterpretedChunk& prefixChunk = interpretedChunk.prefixChunks[i];
            originalForm += env.getCharsetConverter().toString(prefixChunk.originalCodepoints);
            const unsigned char* ptr = prefixChunk.interpsGroup.ptr;
            MorphInterpretation mi = this->decodeMorphInterpretation(0, 0, originalForm, string(""), prefixChunk, ptr);
            decodedForm += mi.getLemma();
        }
    }

    MorphInterpretation decodeMorphInterpretation(
            unsigned int startNode, unsigned int endNode,
            const string& orth,
            const string& lemmaPrefix,
            const InterpretedChunk& chunk,
            const unsigned char*& ptr) const {
        string lemma = lemmaPrefix;
        EncodedInterpretation ei = this->decodeInterp(ptr);
        this->decodeForm(chunk.lowercaseCodepoints, ei.value, lemma);
        return MorphInterpretation(
                startNode, endNode,
                orth, lemma,
                ei.tag,
                ei.nameClassifier,
                env.getTagset(),
                env.getCharsetConverter());
    }

    void decodeLemma(const unsigned char*& ptr, EncodedForm& lemma) const {
        lemma.suffixToCut = *ptr;
        ptr++;
        lemma.suffixToAdd = (const char*) ptr;
        ptr += strlen((const char*) ptr) + 1;
        assert(lemma.casePattern.size() == 0);
        //    lemma.casePattern.resize(MAX_WORD_SIZE, false);
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
                    lemma.casePattern.push_back(true);
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
                    lemma.casePattern.resize(idx + 1, false);
                    lemma.casePattern[idx] = true;
                }
                break;
        }
    }

    EncodedInterpretation decodeInterp(const unsigned char*& ptr) const {
        EncodedInterpretation interp;
        decodeLemma(ptr, interp.value);
        interp.tag = ntohs(*(reinterpret_cast<const uint16_t*> (ptr)));
        ptr += 2;
        interp.nameClassifier = *ptr;
        ptr++;
        return interp;
    }
};

class InterpretedChunksDecoder4Generator : public InterpretedChunksDecoder {
public:

    InterpretedChunksDecoder4Generator(const Environment& env) : InterpretedChunksDecoder(env) {
    }

    void decode(
            unsigned int startNode,
            unsigned int endNode,
            const InterpretedChunk& interpretedChunk,
            std::vector<MorphInterpretation>& out) const {
        //        string orth;
        //        string lemma;
        //        convertPrefixes(interpretedChunk, lemma, orth);
        //        size_t orthLength = orth.length();
        //        lemma += env.getCharsetConverter().toString(interpretedChunk.originalCodepoints);
        //        for (unsigned int i = 0; i < interpretedChunk.interpsGroup.interps.size(); i++) {
        //            const EncodedInterpretation& ei = interpretedChunk.interpsGroup.interps[i];
        //            decodeForm(
        //                    interpretedChunk.originalCodepoints,
        //                    ei.value,
        //                    orth);
        //            out.push_back(MorphInterpretation(
        //                    startNode, endNode,
        //                    orth, lemma,
        //                    ei.tag,
        //                    ei.nameClassifier,
        //                    env.getTagset(),
        //                    env.getCharsetConverter()));
        //            orth.erase(orthLength);
        //        }
    }

private:

    void decodeForm(
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
            uint32_t cp = UTF8CharsetConverter().next(suffixPtr, suffixEnd);
            env.getCharsetConverter().append(cp, res);
        }
    }
};

#endif	/* INTERPSGROUPDECODER_HPP */

