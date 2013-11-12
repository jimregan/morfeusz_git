/* 
 * File:   _simple_fsa_impl.hpp
 * Author: mlenart
 *
 * Created on October 20, 2013, 12:25 PM
 */

#ifndef _SIMPLE_FSA_IMPL_HPP
#define	_SIMPLE_FSA_IMPL_HPP

#include <cstring>
#include <algorithm>
#include <utility>
#include <iostream>
#include <vector>
#include <netinet/in.h>
#include "utils.hpp"
#include "const.hpp"

using namespace std;
//static const unsigned int FSA_OFFSET = 6;

template <class T>
bool FSA<T>::tryToRecognize(const char* input, T& value) const {
    State<T> currState = this->getInitialState();
    int i = 0;
    while (!currState.isSink() && input[i] != '\0') {
#ifdef DEBUG_BUILD
        cerr << "proceed to next " << input[i] << endl;
#endif
        currState.proceedToNext(input[i]);
        i++;
    }
    // input[i] == '\0'
    //    currState.proceedToNext(0);

    if (currState.isAccepting()) {
        value = currState.getValue();
#ifdef DEBUG_BUILD
        cerr << "RECOGNIZED " << input << endl;
#endif
        return true;
    } else {
        return false;
    }
}

template <class T>
FSA<T>::FSA(const unsigned char* initialStatePtr, const Deserializer<T>& deserializer)
    : initialStatePtr(initialStatePtr), deserializer(deserializer) {
    
}

template <class T>
State<T> FSA<T>::getInitialState() const {
    return State<T>(*this);
}

template <class T>
FSA<T>* FSA<T>::getFSA(const unsigned char* ptr, const Deserializer<T>& deserializer) {
    
    uint32_t magicNumber = ntohl(*((uint32_t*) ptr));
    if (magicNumber != MAGIC_NUMBER) {
        throw FSAException("Invalid magic number");
    }
    
    uint8_t versionNum = *(ptr + VERSION_NUM_OFFSET);
    if (versionNum != VERSION_NUM) {
        throw FSAException(string("Invalid version number: ") + to_string(versionNum) + ", should be: " + to_string(VERSION_NUM));
    }
    
    uint8_t implementationNum = *(ptr + IMPLEMENTATION_NUM_OFFSET);
    
    uint32_t additionalDataSize = ntohl(*(reinterpret_cast<const uint32_t*>(ptr + ADDITIONAL_DATA_SIZE_OFFSET)));
    const unsigned char* startPtr = ptr + ADDITIONAL_DATA_OFFSET + additionalDataSize;
    switch (implementationNum) {
        case 0:
            return new SimpleFSA<T>(startPtr, deserializer);
        case 1:
            return new CompressedFSA1<T>(startPtr, deserializer);
        case 2:
            return new CompressedFSA2<T>(startPtr, deserializer);
        default:
            throw FSAException(string("Invalid implementation number: ") + to_string(versionNum) + ", should be: " + to_string(VERSION_NUM));
    }
}

#endif	/* _SIMPLE_FSA_IMPL_HPP */
