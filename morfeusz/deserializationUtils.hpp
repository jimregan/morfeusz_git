/* 
 * File:   deserializationUtils.hpp
 * Author: lennyn
 *
 * Created on April 4, 2014, 6:07 PM
 */

#ifndef DESERIALIZATIONUTILS_HPP
#define	DESERIALIZATIONUTILS_HPP

#include "endianness.hpp"
#include <iostream>

inline uint16_t readInt16(const unsigned char*& currPtr) {
    uint16_t res = htons(*reinterpret_cast<const uint16_t*>(currPtr));
    currPtr += 2;
    return res;
}

inline std::string readString(const unsigned char*& currPtr) {
    std::string res(reinterpret_cast<const char*>(currPtr));
    currPtr += res.length();
    currPtr++;
    return res;
}

inline void readTags(const unsigned char*& currPtr, std::vector<std::string>& tags) {
    tags.clear();
    tags.resize(65536);
    uint16_t tagsNum = readInt16(currPtr);
    for (unsigned int i = 0; i < tagsNum; i++) {
        unsigned int tagNum = readInt16(currPtr);
        tags[tagNum] = readString(currPtr);
    }
}

#endif	/* DESERIALIZATIONUTILS_HPP */

