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
    
    virtual ~InterpretedChunksDecoder() {}

protected:
    
    void convertPrefixes(const InterpretedChunk& interpretedChunk, std::string& originalForm, std::string& decodedForm) const {
        for (unsigned int i = 0; i < interpretedChunk.prefixChunks.size(); i++) {
            const InterpretedChunk& prefixChunk = interpretedChunk.prefixChunks[i];
            originalForm += env.getCharsetConverter().toString(prefixChunk.originalCodepoints);
            decodeForm(
                    prefixChunk.lowercaseCodepoints,
                    prefixChunk.interpsGroup.interps[0].value,
                    decodedForm);
        }
    }
    
    virtual void decodeForm(
            const std::vector<uint32_t>& orth,
            const EncodedForm& form,
            std::string& res) const = 0;

    const Environment& env;
};

class InterpretedChunksDecoder4Analyzer : public InterpretedChunksDecoder {

public:
    InterpretedChunksDecoder4Analyzer(const Environment& env): InterpretedChunksDecoder(env) {}

    void decode(
            unsigned int startNode,
            unsigned int endNode,
            const InterpretedChunk& interpretedChunk,
            std::vector<MorphInterpretation>& out) const {
        string orth;
        string lemma;
        convertPrefixes(interpretedChunk, orth, lemma);
        orth += env.getCharsetConverter().toString(interpretedChunk.originalCodepoints);
        for (unsigned int i = 0; i < interpretedChunk.interpsGroup.interps.size(); i++) {
            const EncodedInterpretation& ei = interpretedChunk.interpsGroup.interps[i];
            decodeForm(
                    interpretedChunk.lowercaseCodepoints,
                    ei.value,
                    lemma);
            out.push_back(MorphInterpretation(
                    startNode, endNode,
                    orth, lemma,
                    ei.tag,
                    ei.nameClassifier,
                    env.getTagset(),
                    env.getCharsetConverter()));
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
};

class InterpretedChunksDecoder4Generator : public InterpretedChunksDecoder {

public:
    InterpretedChunksDecoder4Generator(const Environment& env): InterpretedChunksDecoder(env) {}

    void decode(
            unsigned int startNode,
            unsigned int endNode,
            const InterpretedChunk& interpretedChunk,
            std::vector<MorphInterpretation>& out) const {
        string orth;
        string lemma;
        convertPrefixes(interpretedChunk, lemma, orth);
        lemma += env.getCharsetConverter().toString(interpretedChunk.originalCodepoints);
        for (unsigned int i = 0; i < interpretedChunk.interpsGroup.interps.size(); i++) {
            const EncodedInterpretation& ei = interpretedChunk.interpsGroup.interps[i];
            decodeForm(
                    interpretedChunk.originalCodepoints,
                    ei.value,
                    orth);
            out.push_back(MorphInterpretation(
                    startNode, endNode,
                    orth, lemma,
                    ei.tag,
                    ei.nameClassifier,
                    env.getTagset(),
                    env.getCharsetConverter()));
        }
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

