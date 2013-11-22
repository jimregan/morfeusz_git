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

class InterpretedChunksDecoder {
public:

    InterpretedChunksDecoder(const Tagset& tagset, const CharsetConverter& charsetConverter)
    : tagset(tagset), charsetConverter(charsetConverter) {

    }

    template <class OutputIterator>
    OutputIterator decode(
            unsigned int startNode,
            unsigned int endNode,
            const InterpretedChunk& interpretedChunk,
            OutputIterator out) {
        string orth = this->toString(interpretedChunk.originalCodepoints);
        for (const EncodedInterpretation& ei : interpretedChunk.interpsGroup.interps) {
            string lemma = convertLemma(
                    this->toString(interpretedChunk.lowercaseCodepoints),
                    ei.lemma);
            *out = MorphInterpretation(
                startNode, endNode,
                orth, lemma,
                ei.tag, ei.nameClassifier,
                tagset);
            ++out;
        }
        return out;
    }

    std::string toString(std::vector<uint32_t> codepoints) {
        std::string res;
        for (uint32_t cp : codepoints) {
            charsetConverter.append(cp, res);
        }
        return res;
    }

private:

    string convertLemma(
            const string& orth,
            const EncodedLemma& lemma) {
        string res(orth);
        res.erase(
                res.end() - lemma.suffixToCut,
                res.end());
        res.append(lemma.suffixToAdd);
        return res;
    }

    const Tagset& tagset;
    const CharsetConverter& charsetConverter;
};

#endif	/* INTERPSGROUPDECODER_HPP */

