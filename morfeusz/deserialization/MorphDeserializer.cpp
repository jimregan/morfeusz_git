/* 
 * File:   MorphDeserializer.cpp
 * Author: mlenart
 * 
 * Created on 16 maj 2014, 12:46
 */

#include "MorphDeserializer.hpp"
#include "deserialization/deserializationUtils.hpp"

MorphDeserializer::MorphDeserializer() {
}

MorphDeserializer::~MorphDeserializer() {
}

long MorphDeserializer::deserialize(
        const unsigned char* ptr,
        InterpsGroupsReader& res) const {
    uint16_t size = readInt16(ptr);
    res = InterpsGroupsReader(ptr, size);
    return size + 2;
}
