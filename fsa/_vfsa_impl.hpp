/* 
 * File:   _vfsa_impl.hpp
 * Author: lennyn
 *
 * Created on October 29, 2013, 9:57 PM
 */

#ifndef _VFSA_IMPL_HPP
#define	_VFSA_IMPL_HPP

#include <algorithm>
#include <utility>
#include <iostream>
#include <netinet/in.h>
#include "fsa.hpp"

using namespace std;

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

struct VTransitionData {
    unsigned label : 5;
    unsigned offsetSize : 2;
    unsigned last : 1;
};

#pragma pack(pop)   /* restore original alignment from stack */

template <class T>
int FSAImpl<T>::getMagicNumberOffset() {
    return 0;
}

template <class T>
int FSAImpl<T>::getVersionNumOffset() {
    return getMagicNumberOffset() + sizeof (MAGIC_NUMBER);
}

template <class T>
int FSAImpl<T>::getPopularCharsOffset() {
    return getVersionNumOffset() + sizeof (VERSION_NUM);
}

template <class T>
int FSAImpl<T>::getInitialStateOffset() {
    return getPopularCharsOffset() + POPULAR_CHARS_NUM;
}

template <class T>
vector<unsigned char> FSAImpl<T>::initializeChar2PopularCharIdx(const unsigned char* ptr) {
    vector<unsigned char> res(256, FSAImpl<bool>::POPULAR_CHARS_NUM);
    const unsigned char* popularChars = ptr + getPopularCharsOffset();
    for (unsigned int i = 0; i < POPULAR_CHARS_NUM; i++) {
        res[popularChars[i]] = i;
    }
    return res;
}

template <class T>
FSAImpl<T>::FSAImpl(const unsigned char* ptr, const Deserializer<T>& deserializer)
: FSA<T>(ptr + getInitialStateOffset(), deserializer),
char2PopularCharIdx(initializeChar2PopularCharIdx(ptr)) {
    uint32_t magicNumber = ntohl(*((uint32_t*) ptr + getMagicNumberOffset()));
    if (magicNumber != MAGIC_NUMBER) {
        throw FSAException("Invalid magic number");
    }
    unsigned char versionNum = *(ptr + getVersionNumOffset());
    if (versionNum != VERSION_NUM) {
        throw FSAException("Invalid version number");
    }
    cerr << "initial state offset " << getInitialStateOffset() << endl;
}

template <class T>
FSAImpl<T>::~FSAImpl() {

}

template <class T>
void FSAImpl<T>::proceedToNext(const char c, State<T>& state) const {
    //    if (c <= 'z' && 'a' <= c)
    //        cerr << "NEXT " << c << " from " << state.getOffset() << endl;
    //    else
    //        cerr << "NEXT " << (short) c << " from " << state.getOffset() << endl;
    const unsigned char* fromPointer = this->startPtr + state.getOffset();
    int transitionsTableOffset = 0;
    if (state.isAccepting()) {
        transitionsTableOffset += state.getValueSize();
        cerr << "transitionsTableOffset " << transitionsTableOffset + state.getOffset() << " because value is " << state.getValue() << endl;
    }
    const unsigned char* currPtr = fromPointer + transitionsTableOffset;
    bool found = false;
    bool failed = false;
    unsigned char shortLabel = char2PopularCharIdx[(unsigned char) c];
    cerr << "NEXT " << c << " " << (int) shortLabel << endl;
    VTransitionData td;
    while (!found && !failed) {
        td = *((VTransitionData*) currPtr);
        cerr << "transition at " << (currPtr - this->startPtr) << endl;
        cerr << "short label: " << (int) td.label << endl;
        if (td.label == shortLabel) {
            if (td.label != POPULAR_CHARS_NUM) {
                found = true;
                currPtr++;
            }
            else {
                currPtr++;
                char realLabel = (char) *currPtr;
                cerr << "full label: " << realLabel << endl;
                if (realLabel != c) {
                    failed = td.last;
                    currPtr += td.offsetSize + 1;
                } else {
                    found = true;
                    currPtr++;
                }
            }
        } else if (td.last) {
            cerr << "last" << endl;
            failed = true;
        } else {
            if (td.label == POPULAR_CHARS_NUM) {
                currPtr++;
            }
            currPtr += td.offsetSize + 1;
        }
    }

    if (found) {
        // currPtr points at the offset
        // or next state (iff offset==0)
        int offsetFromHere = 0;
        cerr << "offset size " << td.offsetSize << endl;
        for (int i = 0; i < td.offsetSize; i++) {
            offsetFromHere <<= 8;
            cerr << "offset from here " << offsetFromHere << endl;
            offsetFromHere += *currPtr;
            if (i + 1 < td.offsetSize)
                currPtr++;
            cerr << "offset from here " << offsetFromHere << endl;
        }
        currPtr += offsetFromHere;
        cerr << "offset " << currPtr - this->startPtr << endl;
        bool accepting = c == '\0';
        if (accepting) {
            T value;
            int valueSize = this->deserializer.deserialize(currPtr, value);
            currPtr += valueSize;
            state.setNext(currPtr - this->startPtr, value, valueSize);
        } else {
            state.setNext(currPtr - this->startPtr);
        }
    } else {
        state.setNextAsSink();
    }
}

#endif	/* _VFSA_IMPL_HPP */

