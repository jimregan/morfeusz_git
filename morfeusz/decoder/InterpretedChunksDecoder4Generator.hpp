/* 
 * File:   InterpretedChunksDecoder4Generator.hpp
 * Author: mlenart
 *
 * Created on 15 maj 2014, 15:28
 */

#ifndef INTERPRETEDCHUNKSDECODER4GENERATOR_HPP
#define	INTERPRETEDCHUNKSDECODER4GENERATOR_HPP

#include "InterpretedChunksDecoder.hpp"

class InterpretedChunksDecoder4Generator : public InterpretedChunksDecoder {
public:

    InterpretedChunksDecoder4Generator(const Environment& env);

    void decode(
            unsigned int startNode,
            unsigned int endNode,
            const InterpretedChunk& interpretedChunk,
            std::vector<MorphInterpretation>& out) const;

private:

    void convertPrefixes(const InterpretedChunk& interpretedChunk, std::string& orthPrefix, std::string& lemma) const;

    MorphInterpretation decodeMorphInterpretation(
            unsigned int startNode, unsigned int endNode,
            const string& orthPrefix,
            const string& lemma,
            const InterpretedChunk& chunk,
            const unsigned char*& ptr) const;

    void decodeForm(
            const vector<uint32_t>& lemma,
            const EncodedForm& orth,
            string& res) const;

    EncodedInterpretation deserializeInterp(const unsigned char*& ptr) const;
    
    mutable std::vector<uint32_t> codepoints;
};


#endif	/* INTERPRETEDCHUNKSDECODER4GENERATOR_HPP */

