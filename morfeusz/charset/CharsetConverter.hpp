/* 
 * File:   EncodingConverter.hpp
 * Author: mlenart
 *
 * Created on 14 listopad 2013, 17:28
 */

#ifndef ENCODINGCONVERTER_HPP
#define	ENCODINGCONVERTER_HPP

#include <inttypes.h>
#include <string>
#include <vector>
#include <map>

class CharsetConverter {
public:
    uint32_t peek(const char* it, const char* end) const;
    virtual uint32_t next(const char*& it, const char* end) const = 0;
    virtual void append(uint32_t cp, std::string& result) const = 0;
    virtual std::string fromUTF8(const std::string& input) const;

    std::string toString(const std::vector<uint32_t>& codepoints) const;
    
    virtual ~CharsetConverter();
private:
};

class UTF8CharsetConverter : public CharsetConverter {
public:
    uint32_t next(const char*& it, const char* end) const;
    void append(uint32_t cp, std::string& result) const;
    //    std::string fromUTF8(const std::string& input) const;
private:
};

/*
 * Converter that uses a simple conversion table
 */
class OneByteCharsetConverter : public CharsetConverter {
public:
    explicit OneByteCharsetConverter(const uint32_t* array);
    uint32_t next(const char*& it, const char* end) const;
    void append(uint32_t cp, std::string& result) const;
private:
    const uint32_t* array;
    const std::vector<char> codepoint2Char;
};

class ISO8859_2_CharsetConverter : public OneByteCharsetConverter {
public:
    ISO8859_2_CharsetConverter();
private:
};

class Windows_1250_CharsetConverter : public OneByteCharsetConverter {
public:
    Windows_1250_CharsetConverter();
private:
};

class CP852_CharsetConverter : public OneByteCharsetConverter {
public:
    CP852_CharsetConverter();
private:
};

#endif	/* ENCODINGCONVERTER_HPP */

