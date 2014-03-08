/* 
 * File:   InterpsGroupDecoder.hpp
 * Author: mlenart
 *
 * Created on November 22, 2013, 10:35 PM
 */

#ifndef INTERPSGROUPDECODER_HPP
#define	INTERPSGROUPDECODER_HPP

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

    template <class OutputIterator>
    OutputIterator decode(
            unsigned int startNode,
            unsigned int endNode,
            const InterpretedChunk& interpretedChunk,
            OutputIterator out) {
        string orth;
        string lemmaPrefix;
        for (unsigned int i = 0; i < interpretedChunk.prefixChunks.size(); i++) {
            const InterpretedChunk& prefixChunk = interpretedChunk.prefixChunks[i];
            orth += env.getCharsetConverter().toString(prefixChunk.originalCodepoints);
            lemmaPrefix += convertLemma(
                    prefixChunk.lowercaseCodepoints,
                    prefixChunk.interpsGroup.interps[0].lemma);
        }
        orth += env.getCharsetConverter().toString(interpretedChunk.originalCodepoints);
        for (unsigned int i = 0; i < interpretedChunk.interpsGroup.interps.size(); i++) {
            const EncodedInterpretation& ei = interpretedChunk.interpsGroup.interps[i];
            string lemma = lemmaPrefix + convertLemma(
                    interpretedChunk.lowercaseCodepoints,
                    ei.lemma);
            *out = MorphInterpretation(
                    startNode, endNode,
                    orth, lemma,
                    ei.tag,
                    ei.nameClassifier,
                    env.getAnalyzerTagset(),
                    env.getCharsetConverter());
            ++out;
        }
        return out;
    }

private:

    string convertLemma(
            const vector<uint32_t>& orth,
            const EncodedLemma& lemma) {
        string res;
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
        return res;
    }

    const Environment& env;
};

#endif	/* INTERPSGROUPDECODER_HPP */

