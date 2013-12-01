/* 
 * File:   EncodingConverter.hpp
 * Author: mlenart
 *
 * Created on 14 listopad 2013, 17:28
 */

#ifndef ENCODINGCONVERTER_HPP
#define	ENCODINGCONVERTER_HPP

#include <cstdint>
#include <string>

class CharsetConverter {
public:
    virtual uint32_t peek(const char*& it, const char* end) const = 0;
    virtual uint32_t next(const char*& it, const char* end) const = 0;
    virtual void append(uint32_t cp, std::string& result) const = 0;
private:
};

class UTF8CharsetConverter: public CharsetConverter {
public:
    uint32_t peek(const char*& it, const char* end) const;
    uint32_t next(const char*& it, const char* end) const;
    void append(uint32_t cp, std::string& result) const;
private:
};

class UTF16CharsetConverter: public CharsetConverter {
public:
    uint32_t peek(const char*& it, const char* end) const;
    uint32_t next(const char*& it, const char* end) const;
    void append(uint32_t cp, std::string& result) const;
private:
};

class UTF32CharsetConverter: public CharsetConverter {
public:
    uint32_t peek(const char*& it, const char* end) const;
    uint32_t next(const char*& it, const char* end) const;
    void append(uint32_t cp, std::string& result) const;
private:
};

class ISO8859_2_CharsetConverter: public CharsetConverter {
public:
    uint32_t peek(const char*& it, const char* end) const;
    uint32_t next(const char*& it, const char* end) const;
    void append(uint32_t cp, std::string& result) const;
private:
};

#endif	/* ENCODINGCONVERTER_HPP */
