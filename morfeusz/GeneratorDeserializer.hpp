/* 
 * File:   GeneratorDeserializer.hpp
 * Author: mlenart
 *
 * Created on 20 styczeń 2014, 17:14
 */

#ifndef GENERATORDESERIALIZER_HPP
#define	GENERATORDESERIALIZER_HPP

#include <string>
#include <vector>
#include "fsa/fsa.hpp"
#include "MorphInterpretation.hpp"

class GeneratorDeserializer: public Deserializer< std::vector<MorphInterpretation> > {
public:
    GeneratorDeserializer(const std::string& lemma);
    void setCurrentLemma(const std::string& lemma);
    long deserialize(
        const unsigned char* ptr, 
        std::vector<MorphInterpretation>& interps) const;
    virtual ~GeneratorDeserializer();
private:
    const std::string* lemma;
};

#endif	/* GENERATORDESERIALIZER_HPP */

