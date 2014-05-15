/* 
 * File:   InterpretedChunksDecoder4Analyzer.hpp
 * Author: mlenart
 *
 * Created on 15 maj 2014, 15:28
 */

#ifndef INTERPRETEDCHUNKSDECODER4ANALYZER_HPP
#define	INTERPRETEDCHUNKSDECODER4ANALYZER_HPP

#include "InterpretedChunksDecoder.hpp"

class InterpretedChunksDecoder4Analyzer : public InterpretedChunksDecoder {
public:

    InterpretedChunksDecoder4Analyzer(const Environment& env);

    void decode(
            unsigned int startNode,
            unsigned int endNode,
            const InterpretedChunk& interpretedChunk,
            std::vector<MorphInterpretation>& out) const;

private:

    void decodeLemma(
            const vector<uint32_t>& orth,
            const EncodedForm& lemma,
            bool forPrefix,
            string& res) const;

    void deserializeEncodedForm(const unsigned char*& ptr, unsigned char compressionByte, EncodedForm& encodedForm) const;

    EncodedInterpretation deserializeEncodedInterp(const unsigned char*& ptr, unsigned char compressionByte) const;

    void decodeMorphInterpretation(
            unsigned int startNode, unsigned int endNode,
            const string& orth,
            const string& lemmaPrefix,
            const InterpretedChunk& chunk,
            bool forPrefix,
            const unsigned char*& ptr,
            std::vector<MorphInterpretation>& out) const;

    bool convertPrefixes(const InterpretedChunk& interpretedChunk, std::string& orth, std::string& lemmaPrefix) const;
    
    mutable std::vector<uint32_t> orthCodepoints;
    mutable std::vector<uint32_t> normalizedCodepoints;
};

#endif	/* INTERPRETEDCHUNKSDECODER4ANALYZER_HPP */

