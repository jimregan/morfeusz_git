/* 
 * File:   SegrulesDeserializer.cpp
 * Author: mlenart
 * 
 * Created on 25 luty 2014, 16:16
 */

#include "SegrulesDeserializer.hpp"

SegrulesDeserializer::SegrulesDeserializer() {
}

long SegrulesDeserializer::deserialize(const unsigned char* ptr, unsigned char& object) const {
    object = *ptr;
    return 1;
}

SegrulesDeserializer::~SegrulesDeserializer() {
}

