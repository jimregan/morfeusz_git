/* 
 * File:   EncodingConverter.cpp
 * Author: mlenart
 * 
 * Created on 14 listopad 2013, 17:28
 */

#include "utf8.h"
#include "CharsetConverter.hpp"

uint32_t UTF8CharsetConverter::next(const char*& it, const char* end) const {
    return utf8::next(it, end);
}
char* UTF8CharsetConverter::append(uint32_t cp, char* result) const {
    return utf8::append(cp, result);
}
