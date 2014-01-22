/* 
 * File:   GeneratorDeserializer.hpp
 * Author: mlenart
 *
 * Created on 20 styczeń 2014, 17:14
 */

#ifndef SYNTHDESERIALIZER_HPP
#define	SYNTHDESERIALIZER_HPP

#include <string>
#include <vector>
#include "fsa/fsa.hpp"
#include "Tagset.hpp"
#include "EncodedGeneratorInterpretation.hpp"
#include "Environment.hpp"

class GeneratorDeserializer: public Deserializer< std::vector<EncodedGeneratorInterpretation> > {
public:
    explicit GeneratorDeserializer(const Environment& env);
    long deserialize(
        const unsigned char* ptr, 
        std::vector<EncodedGeneratorInterpretation>& interps) const;
    virtual ~GeneratorDeserializer();
private:
    const Environment& env;
    
    void deserializeInterp(const unsigned char*& ptr, EncodedGeneratorInterpretation& interp) const;
    void deserializeOrth(const unsigned char*& ptr, EncodedOrth& orth) const;
};

#endif	/* SYNTHDESERIALIZER_HPP */

