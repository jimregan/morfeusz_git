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

class InterpretedChunksDecoder {
public:

    InterpretedChunksDecoder(
            const Tagset& tagset,
            const CharsetConverter& charsetConverter,
            const CaseConverter& caseConverter)
    : tagset(tagset),
    charsetConverter(charsetConverter),
    utf8CharsetConverter(),
    caseConverter(caseConverter) {

    }

    template <class OutputIterator>
    OutputIterator decode(
            unsigned int startNode,
            unsigned int endNode,
            const InterpretedChunk& interpretedChunk,
            OutputIterator out) {
        string orth = charsetConverter.toString(interpretedChunk.originalCodepoints);
        for (const EncodedInterpretation& ei : interpretedChunk.interpsGroup.interps) {
            string lemma = convertLemma(
                    interpretedChunk.lowercaseCodepoints,
                    ei.lemma);
            *out = MorphInterpretation(
                    startNode, endNode,
                    orth, lemma,
                    ei.tag,
                    ei.nameClassifier,
                    tagset,
                    charsetConverter);
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
                    ? this->caseConverter.toTitle(orth[i])
                    : orth[i];
            charsetConverter.append(cp, res);
        }
        const char* suffixPtr = lemma.suffixToAdd.c_str();
        const char* suffixEnd = suffixPtr + lemma.suffixToAdd.length();
        while (suffixPtr != suffixEnd) {
            uint32_t cp = utf8CharsetConverter.next(suffixPtr, suffixEnd);
            charsetConverter.append(cp, res);
        }
        //        string res(orth);
        //        res.erase(
        //                res.end() - lemma.suffixToCut,
        //                res.end());
        //        res.append(lemma.suffixToAdd);
        return res;
    }

    const Tagset& tagset;
    const CharsetConverter& charsetConverter;
    const UTF8CharsetConverter utf8CharsetConverter;
    const CaseConverter& caseConverter;
};

#endif	/* INTERPSGROUPDECODER_HPP */

