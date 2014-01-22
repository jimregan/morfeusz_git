/* 
 * File:   GeneratorDeserializer.cpp
 * Author: mlenart
 * 
 * Created on 20 styczeń 2014, 17:14
 */

#include "GeneratorDeserializer.hpp"
#include "EncodedGeneratorInterpretation.hpp"

using namespace std;

GeneratorDeserializer::GeneratorDeserializer(const Environment& env)
: env(env) {

}

void GeneratorDeserializer::deserializeOrth(const unsigned char*& ptr, EncodedOrth& orth) const {
       // XXX uważać na poprawność danych
    orth.suffixToCut = *ptr;
    ptr++;
    orth.suffixToAdd = (const char*) ptr;
    ptr += strlen((const char*) ptr) + 1;
}

void GeneratorDeserializer::deserializeInterp(const unsigned char*& ptr, EncodedGeneratorInterpretation& interp) const {
    deserializeOrth(ptr, interp.orth);
    interp.tag = ntohs(*(reinterpret_cast<const uint16_t*> (ptr)));
    ptr += 2;
    interp.nameClassifier = *ptr;
    ptr++;
}

long GeneratorDeserializer::deserialize(
        const unsigned char* ptr,
        std::vector<EncodedGeneratorInterpretation>& interps) const {
    const unsigned char* currPtr = ptr;
    uint8_t interpsNum = *ptr;
    interps.clear();
    interps.reserve(interpsNum);
    currPtr++;
    for (unsigned int i = 0; i < interpsNum; ++i) {
        EncodedGeneratorInterpretation interp;
        this->deserializeInterp(currPtr, interp);
        interps.push_back(interp);
    }
    return currPtr - ptr;
}

GeneratorDeserializer::~GeneratorDeserializer() {

}
