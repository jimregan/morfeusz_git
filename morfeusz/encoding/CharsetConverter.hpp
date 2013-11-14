/* 
 * File:   EncodingConverter.hpp
 * Author: mlenart
 *
 * Created on 14 listopad 2013, 17:28
 */

#ifndef ENCODINGCONVERTER_HPP
#define	ENCODINGCONVERTER_HPP

class CharsetConverter {
public:
    virtual uint32_t next(const char*& it, const char* end) const = 0;
    virtual const char* append(uint32_t cp, const char* result) const = 0;
private:
};

class UTF8CharsetConverter: public CharsetConverter {
public:
    uint32_t next(const char*& it, const char* end) const;
    const char* append(uint32_t cp, const char* result) const;
private:
};

class UTF16CharsetConverter: public CharsetConverter {
public:
    uint32_t next(const char*& it, const char* end) const;
    const char* append(uint32_t cp, const char* result) const;
private:
};

class UTF32CharsetConverter: public CharsetConverter {
public:
    uint32_t next(const char*& it, const char* end) const;
    const char* append(uint32_t cp, const char* result) const;
private:
};

class ISO8859_2_CharsetConverter: public CharsetConverter {
public:
    uint32_t next(const char*& it, const char* end) const;
    const char* append(uint32_t cp, const char* result) const;
private:
};

#endif	/* ENCODINGCONVERTER_HPP */

