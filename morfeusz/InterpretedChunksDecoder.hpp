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

    virtual ~InterpretedChunksDecoder() {
    }
    
    virtual void decode(
            unsigned int startNode,
            unsigned int endNode,
            const InterpretedChunk& interpretedChunk,
            std::vector<MorphInterpretation>& out) const = 0;

protected:
    
    virtual MorphInterpretation decodeMorphInterpretation(
            unsigned int startNode, unsigned int endNode,
            const string& orth,
            const string& lemmaPrefix,
            const InterpretedChunk& chunk,
            const unsigned char*& ptr) const = 0;

    virtual void decodeForm(
            const std::vector<uint32_t>& orth,
            const EncodedForm& form,
            std::string& res) const = 0;
    
    EncodedInterpretation deserializeInterp(const unsigned char*& ptr) const {
        EncodedInterpretation interp;
        deserializeEncodedForm(ptr, interp.value);
        interp.tag = ntohs(*(reinterpret_cast<const uint16_t*> (ptr)));
        ptr += 2;
        interp.nameClassifier = *ptr;
        ptr++;
        return interp;
    }
    
    virtual void deserializeEncodedForm(const unsigned char*& ptr, EncodedForm& encodedForm) const = 0;

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

    void deserializeEncodedForm(const unsigned char*& ptr, EncodedForm& encodedForm) const {
        encodedForm.suffixToCut = *ptr;
        ptr++;
        encodedForm.suffixToAdd = (const char*) ptr;
        ptr += strlen((const char*) ptr) + 1;
        assert(encodedForm.casePattern.size() == 0);
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
                    encodedForm.casePattern.push_back(true);
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
                    encodedForm.casePattern.resize(idx + 1, false);
                    encodedForm.casePattern[idx] = true;
                }
                break;
        }
    }
private:
    
    MorphInterpretation decodeMorphInterpretation(
            unsigned int startNode, unsigned int endNode,
            const string& orth,
            const string& lemmaPrefix,
            const InterpretedChunk& chunk,
            const unsigned char*& ptr) const {
        string lemma = lemmaPrefix;
        EncodedInterpretation ei = this->deserializeInterp(ptr);
        this->decodeForm(chunk.lowercaseCodepoints, ei.value, lemma);
        return MorphInterpretation(
                startNode, endNode,
                orth, lemma,
                ei.tag,
                ei.nameClassifier,
                env.getTagset(),
                env.getCharsetConverter());
    }
    
    void convertPrefixes(const InterpretedChunk& interpretedChunk, std::string& orth, std::string& lemmaPrefix) const {
        for (unsigned int i = 0; i < interpretedChunk.prefixChunks.size(); i++) {
            const InterpretedChunk& prefixChunk = interpretedChunk.prefixChunks[i];
            orth += env.getCharsetConverter().toString(prefixChunk.originalCodepoints);
            const unsigned char* ptr = prefixChunk.interpsGroup.ptr;
            MorphInterpretation mi = this->decodeMorphInterpretation(0, 0, orth, string(""), prefixChunk, ptr);
            lemmaPrefix += mi.getLemma();
        }
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
        string orthPrefix;
        string lemma;
        convertPrefixes(interpretedChunk, orthPrefix, lemma);
        lemma += env.getCharsetConverter().toString(interpretedChunk.originalCodepoints);
        const unsigned char* currPtr = interpretedChunk.interpsGroup.ptr;
        while (currPtr - interpretedChunk.interpsGroup.ptr < interpretedChunk.interpsGroup.size) {
            out.push_back(this->decodeMorphInterpretation(startNode, endNode, orthPrefix, lemma, interpretedChunk, currPtr));
        }
    }

private:
    
    void convertPrefixes(const InterpretedChunk& interpretedChunk, std::string& orthPrefix, std::string& lemma) const {
        for (unsigned int i = 0; i < interpretedChunk.prefixChunks.size(); i++) {
            const InterpretedChunk& prefixChunk = interpretedChunk.prefixChunks[i];
            lemma += env.getCharsetConverter().toString(prefixChunk.originalCodepoints);
            const unsigned char* ptr = prefixChunk.interpsGroup.ptr;
            MorphInterpretation mi = this->decodeMorphInterpretation(0, 0, orthPrefix, string(""), prefixChunk, ptr);
            orthPrefix += mi.getOrth();
        }
    }
    
    MorphInterpretation decodeMorphInterpretation(
            unsigned int startNode, unsigned int endNode,
            const string& orthPrefix,
            const string& lemma,
            const InterpretedChunk& chunk,
            const unsigned char*& ptr) const {
        string orth = orthPrefix;
        EncodedInterpretation ei = this->deserializeInterp(ptr);
        this->decodeForm(chunk.originalCodepoints, ei.value, orth);
        return MorphInterpretation(
                startNode, endNode,
                orth, lemma,
                ei.tag,
                ei.nameClassifier,
                env.getTagset(),
                env.getCharsetConverter());
    }

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
    
    void deserializeEncodedForm(const unsigned char*& ptr, EncodedForm& encodedForm) const {
        encodedForm.prefixToAdd = (const char*) ptr;
        ptr += strlen((const char*) ptr) + 1;
        encodedForm.suffixToCut = *ptr;
        ptr++;
        encodedForm.suffixToAdd = (const char*) ptr;
        ptr += strlen((const char*) ptr) + 1;
    }
};

#endif	/* INTERPSGROUPDECODER_HPP */

