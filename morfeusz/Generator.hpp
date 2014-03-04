/* 
 * File:   Generator.hpp
 * Author: mlenart
 *
 * Created on 21 styczeń 2014, 14:38
 */

#ifndef GENERATOR_HPP
#define	GENERATOR_HPP

#include <string>
#include <vector>
#include "charset/CharsetConverter.hpp"
#include "MorphInterpretation.hpp"
#include "Tagset.hpp"
#include "GeneratorDeserializer.hpp"

typedef FSA< std::vector<EncodedGeneratorInterpretation > > SynthFSAType;
typedef State< std::vector<EncodedGeneratorInterpretation > > SynthStateType;

class Generator {
public:
    Generator(
            const unsigned char* ptr, 
            const Environment& env);
    void generate(const std::string& lemma, std::vector<MorphInterpretation>& result) const;
    virtual ~Generator();
private:
//    Generator(const SynthDeserializer& deserializer);
    GeneratorDeserializer deserializer;
    const SynthFSAType* fsa;
    const Environment& env;
    
    std::string decodeOrth(
        const EncodedOrth& orth, 
        const std::vector<uint32_t>& lemmaCodepoints) const;
    
    void decodeRes(
        const std::vector<EncodedGeneratorInterpretation>& encodedRes, 
        const std::string& lemma, 
        const std::vector<uint32_t>& lemmaCodepoints,
        std::vector<MorphInterpretation>& result) const;
    
    void appendString(const string& str, string& res) const;
};

#endif	/* GENERATOR_HPP */

