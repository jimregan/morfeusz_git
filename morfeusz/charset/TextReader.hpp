/* 
 * File:   TextReader.hpp
 * Author: lennyn
 *
 * Created on May 28, 2014, 11:43 AM
 */

#ifndef TEXTREADER_HPP
#define	TEXTREADER_HPP

#include "Environment.hpp"

class TextReader {
public:
    TextReader(const char* inputStart, const char* inputEnd, const Environment& env);
    void markWordStartsHere();
    const char* getWordStartPtr() const;
    const char* getCurrPtr() const;
    const char* getNextPtr();
    const char* getEndPtr() const;
    int getCodepointsRead() const;
    bool isAtEnd() const;
    bool isAtWhitespace();
    uint32_t peek();
    uint32_t normalizedPeek();
    uint32_t next();
    void skipWhitespaces();
    void proceedToEnd();
    virtual ~TextReader();
private:
    int codepointsNum;
    const char* wordStartPtr;
    const char* currPtr;
    const char* inputEnd;
    const Environment& env;
    bool knowsAboutWhitespace;
    bool atWhitespace;
    bool peekIsRead;
    uint32_t thePeek;
    uint32_t theNormalizedPeek;
    const char* ptrAfterThePeek;
};

#endif	/* TEXTREADER_HPP */

