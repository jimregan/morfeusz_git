
#include "SegrulesDeserializer.hpp"
#include "segrules.hpp"
#include "../fsa/fsa.hpp"

FSA<unsigned char>* createSegrulesFSA(const unsigned char* analyzerPtr) {
    static SegrulesDeserializer deserializer;
    return new SimpleFSA<unsigned char>(analyzerPtr, deserializer, true);
}
