/* 
 * File:   charset_utils.hpp
 * Author: lennyn
 *
 * Created on November 15, 2013, 1:57 PM
 */

#ifndef CHARSET_UTILS_HPP
#define	CHARSET_UTILS_HPP

#include <string>
#include <set>
#include <algorithm>
#include "CharsetConverter.hpp"

static inline std::vector<uint32_t> initializeWhitespaces() {
    std::vector<uint32_t> res;
    res.push_back(0x0000); // NULL
    res.push_back(0x0009); // CHARACTER TABULATION
    res.push_back(0x000A); // LINE FEED (LF)
    res.push_back(0x000B); // LINE TABULATION
    res.push_back(0x000C); // FORM FEED (FF)
    res.push_back(0x000D); // CARRIAGE RETURN (CR)
    res.push_back(0x001C); // INFORMATION SEPARATOR FOUR
    res.push_back(0x001D); // INFORMATION SEPARATOR THREE
    res.push_back(0x001E); // INFORMATION SEPARATOR TWO
    res.push_back(0x001F); // INFORMATION SEPARATOR ONE
    res.push_back(0x0020); // SPACE
    res.push_back(0x0085); // NEXT LINE (NEL)
    res.push_back(0x00A0); // NON-BREAKING SPACE
    res.push_back(0x1680); // OGHAM SPACE MARK
    res.push_back(0x180E); // MONGOLIAN VOWEL SEPARATOR
    res.push_back(0x2000); // EN QUAD
    res.push_back(0x2001); // EM QUAD
    res.push_back(0x2002); // EN SPACE
    res.push_back(0x2003); // EM SPACE
    res.push_back(0x2004); // THREE-PER-EM SPACE
    res.push_back(0x2005); // FOUR-PER-EM SPACE
    res.push_back(0x2006); // SIX-PER-EM SPACE
    res.push_back(0x2007); // FIGURE SPACE
    res.push_back(0x2008); // PUNCTUATION SPACE
    res.push_back(0x2009); // THIN SPACE
    res.push_back(0x200A); // HAIR SPACE
    res.push_back(0x2028); // LINE SEPARATOR
    res.push_back(0x2029); // PARAGRAPH SEPARATOR
    res.push_back(0x205F); // MEDIUM MATHEMATICAL SPACE
    res.push_back(0x3000); // IDEOGRAPHIC SPACE
    return res;
}

inline bool isEndOfWord(uint32_t codepoint) {
    static std::vector<uint32_t> whitespaces(initializeWhitespaces());
    return std::binary_search(whitespaces.begin(), whitespaces.end(), codepoint);
    //    return whitespaces.count(codepoint);
}

template <class StateClass>
void feedState(
        StateClass& state,
        int codepoint,
        const CharsetConverter& charsetConverter) {
    std::string chars;
    charsetConverter.append(codepoint, chars);
    for (unsigned int i = 0; i < chars.length(); i++) {
        state.proceedToNext(chars[i]);
    }
}

#endif	/* CHARSET_UTILS_HPP */

