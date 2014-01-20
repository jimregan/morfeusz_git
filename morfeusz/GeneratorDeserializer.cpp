/* 
 * File:   GeneratorDeserializer.cpp
 * Author: mlenart
 * 
 * Created on 20 styczeń 2014, 17:14
 */

#include "GeneratorDeserializer.hpp"

using namespace std;

GeneratorDeserializer::GeneratorDeserializer(const string& lemma)
: lemma(&lemma) {
    
}

void GeneratorDeserializer::setCurrentLemma(const string& lemma) {
    this->lemma = &lemma;
}

long GeneratorDeserializer::deserialize(
        const unsigned char* ptr,
        std::vector<MorphInterpretation>& interps) const {
    
}

GeneratorDeserializer::~GeneratorDeserializer() {
    
}
