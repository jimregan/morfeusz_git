
#include <vector>
#include <iterator>
#include "utf8.h"
#include "CharsetConverter.hpp"
#include "conversion_tables.hpp"

using namespace std;

uint32_t UTF8CharsetConverter::peek(const char*& it, const char* end) const {
    return utf8::peek_next(it, end);
}

uint32_t UTF8CharsetConverter::next(const char*& it, const char* end) const {
    return utf8::next(it, end);
}

void UTF8CharsetConverter::append(uint32_t cp, string& result) const {
    utf8::append(cp, back_inserter(result));
}

uint32_t UTF16CharsetConverter::peek(const char*& it, const char* end) const {
    /*
     * Borrowed from utfcpp
     */

    uint32_t cp = utf8::internal::mask16(*it);
    // Take care of surrogate pairs first
    if (utf8::internal::is_lead_surrogate(cp)) {
        uint32_t trail_surrogate = utf8::internal::mask16(*(it + 1));
        cp = (cp << 10) + trail_surrogate + utf8::internal::SURROGATE_OFFSET;
    }
    return cp;
}

uint32_t UTF16CharsetConverter::next(const char*& it, const char* end) const {

    /*
     * Borrowed from utfcpp
     */

    uint32_t cp = utf8::internal::mask16(*it++);
    // Take care of surrogate pairs first
    if (utf8::internal::is_lead_surrogate(cp)) {
        uint32_t trail_surrogate = utf8::internal::mask16(*it++);
        cp = (cp << 10) + trail_surrogate + utf8::internal::SURROGATE_OFFSET;
    }
    return cp;
}

static void appendWCharToString(uint16_t wchar, string& result) {
    uint8_t b[2] = { 
        static_cast<uint8_t>(wchar & 0x00FF), 
        static_cast<uint8_t>((wchar & 0xFF00) >> 8)};
    char* chars = reinterpret_cast<char*>(b);
    result.push_back(chars[0]);
    result.push_back(chars[1]);
}

void UTF16CharsetConverter::append(uint32_t cp, string& result) const {
    /*
     * borrowed from utfcpp
     */
    if (cp > 0xffff) { //make a surrogate pair
        uint16_t wchar1 = static_cast<uint16_t> ((cp >> 10) + utf8::internal::LEAD_OFFSET);
        uint16_t wchar2 = static_cast<uint16_t> ((cp & 0x3ff) + utf8::internal::TRAIL_SURROGATE_MIN);
        appendWCharToString(wchar1, result);
        appendWCharToString(wchar2, result);
    } else {
        uint16_t wchar = static_cast<uint16_t> (cp);
        appendWCharToString(wchar, result);
    }
}

static vector<char> reverseArray(const uint32_t* array) {
    
}

OneByteCharsetConverter::OneByteCharsetConverter(const uint32_t* array)
: array(array),
codepoint2Char(reverseArray(array))
{
}

uint32_t OneByteCharsetConverter::peek(const char*& it, const char* end) const {
}

uint32_t OneByteCharsetConverter::next(const char*& it, const char* end) const {
}

void OneByteCharsetConverter::append(uint32_t cp, std::string& result) const {
}

ISO8859_2_CharsetConverter::ISO8859_2_CharsetConverter()
: OneByteCharsetConverter(ISO_8859_2_TO_CODEPOINT) {
}

Windows_1250_CharsetConverter::Windows_1250_CharsetConverter()
: OneByteCharsetConverter(WINDOWS_1250_TO_CODEPOINT) {
}
