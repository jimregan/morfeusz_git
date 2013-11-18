/* 
 * File:   charset_utils.hpp
 * Author: lennyn
 *
 * Created on November 15, 2013, 1:57 PM
 */

#ifndef CHARSET_UTILS_HPP
#define	CHARSET_UTILS_HPP

#include <set>

bool isEndOfWord(int codepoint) {
    static std::set<int> whitespaces = { 0x00, 0x0A, 0x20 };
    return whitespaces.count(codepoint) != 0;
}

#endif	/* CHARSET_UTILS_HPP */

