/* 
 * File:   charset_utils.hpp
 * Author: lennyn
 *
 * Created on November 15, 2013, 1:57 PM
 */

#ifndef CHARSET_UTILS_HPP
#define	CHARSET_UTILS_HPP

#include <set>

static inline std::set<int> initializeWhitespaces() {
    std::set<int> res;
    res.insert(0x00);
    res.insert(0x0A);
    res.insert(0x20);
    return res;
}

bool isEndOfWord(int codepoint) {
    static std::set<int> whitespaces(initializeWhitespaces());
    return whitespaces.count(codepoint);
}

#endif	/* CHARSET_UTILS_HPP */

