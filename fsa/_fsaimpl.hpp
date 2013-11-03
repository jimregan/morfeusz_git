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

struct StateData2 {
    unsigned transitionsNum: 6;
    unsigned array : 1;
    unsigned accepting : 1;
};

struct TransitionData2 {
    unsigned offsetSize : 2;
    unsigned shortLabel : 6;
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
    return getPopularCharsOffset() + 256 + 1;
}

template <class T>
vector<unsigned char> FSAImpl<T>::initializeChar2PopularCharIdx(const unsigned char* ptr) {
    return vector<unsigned char>(ptr + getPopularCharsOffset(), ptr + getPopularCharsOffset() + 256);
}

template <class T>
FSAImpl<T>::FSAImpl(const unsigned char* ptr, const Deserializer<T>& deserializer)
: FSA<T>(ptr + getInitialStateOffset(), deserializer),
label2ShortLabel(initializeChar2PopularCharIdx(ptr)) {
    uint32_t magicNumber = ntohl(*((uint32_t*) ptr + getMagicNumberOffset()));
    if (magicNumber != MAGIC_NUMBER) {
        throw FSAException("Invalid magic number");
    }
    unsigned char versionNum = *(ptr + getVersionNumOffset());
    if (versionNum != VERSION_NUM) {
        throw FSAException("Invalid version number");
    }
    //    cerr << "initial state offset " << getInitialStateOffset() << endl;
}

template <class T>
FSAImpl<T>::~FSAImpl() {

}

template <class T>
void FSAImpl<T>::reallyDoProceed(
        const unsigned char* statePtr,
        State<T>& state) const {
//    const unsigned char stateByte = *statePtr;
    StateData2* sd = (StateData2*) statePtr;
    if (sd->accepting) {
//                                            cerr << "ACCEPTING" << endl;
        T object;
        int size = this->deserializer.deserialize(statePtr + 1, object);
        state.setNext(statePtr - this->startPtr, object, size);
    }
    else {
        state.setNext(statePtr - this->startPtr);
    }
}

template <class T>
void FSAImpl<T>::doProceedToNextByList(
        const char c,
        const unsigned char shortLabel,
        const unsigned char* ptr,
        const unsigned int transitionsNum,
        State<T>& state) const {
    register const unsigned char* currPtr = ptr;
    //    TransitionData* foundTransition = (TransitionData*) (fromPointer + transitionsTableOffset);
    bool found = false;
    TransitionData2 td;
    for (unsigned int i = 0; i < transitionsNum; i++) {
        //        const_cast<Counter*>(&counter)->increment(1);
        td = *((TransitionData2*) currPtr);
        if (td.shortLabel == shortLabel) {
            if (shortLabel == 0) {
                currPtr++;
                char label = (char) *currPtr;
                if (label == c) {
                    found = true;
                    break;
                }
                else {
                    currPtr += td.offsetSize + 1;
                }
            } else {
                found = true;
                break;
            }
        } 
        else {
            if (td.shortLabel == 0) {
                currPtr++;
            }
            currPtr += td.offsetSize + 1;
        }
    }
    //        const_cast<Counter*>(&counter)->increment(foundTransition - transitionsStart + 1);
    if (!found) {
//                                cerr << "SINK for " << c << endl;
        state.setNextAsSink();
    } else {
        currPtr++;
//                                        cerr << "offset size " << td.offsetSize << endl;
//                            cerr << "offset " << offset << endl;
        switch (td.offsetSize) {
            case 0:
                break;
            case 1:
                currPtr += *currPtr + 1;
                break;
            case 2:
                currPtr += ntohs(*((uint16_t*) currPtr)) + 2;
                break;
            case 3:
                currPtr += (((unsigned int) ntohs(*((uint16_t*) currPtr))) << 8) + currPtr[2] + 3;
                break;
        }
//                                cerr << "FOUND " << c << " " << currPtr - this->startPtr << endl;
        reallyDoProceed(currPtr, state);
    }
}

template <class T>
void FSAImpl<T>::doProceedToNextByArray(
        const unsigned char shortLabel,
        const uint32_t* ptr,
        State<T>& state) const {
    uint32_t offset = ntohl(ptr[shortLabel]);
    if (offset != 0) {
        const unsigned char* currPtr = this->startPtr + offset;
        reallyDoProceed(currPtr, state);
    }
    else {
        state.setNextAsSink();
    }
}

template <class T>
void FSAImpl<T>::proceedToNext(const char c, State<T>& state) const {
//                    if (c <= 'z' && 'a' <= c)
//                        cerr << "NEXT " << c << " from " << state.getOffset() << endl;
//                    else
//                        cerr << "NEXT " << (short) c << " from " << state.getOffset() << endl;
    const unsigned char* fromPointer = this->startPtr + state.getOffset();
    unsigned char shortLabel = this->label2ShortLabel[(const unsigned char) c];
    unsigned int transitionsTableOffset = 1;
    if (state.isAccepting()) {
        transitionsTableOffset += state.getValueSize();
//                                cerr << "transitionsTableOffset " << transitionsTableOffset + state.getOffset() << " because value is " << state.getValue() << endl;
    }
    StateData2* sd = (StateData2*) (fromPointer);
//                cerr << "transitions num=" << sd->transitionsNum << endl;
    if (sd->array) {
        if (shortLabel > 0) {
            this->doProceedToNextByArray(
                    shortLabel,
                    (uint32_t*) (fromPointer + transitionsTableOffset),
                    state);
        }
        else {
            reallyDoProceed((unsigned char*) fromPointer + transitionsTableOffset + 256, state);
            proceedToNext(c, state);
        }
    } 
    else {
        this->doProceedToNextByList(
                c,
                shortLabel,
                (unsigned char*) (fromPointer + transitionsTableOffset),
                sd->transitionsNum,
                state);
    }
}

#endif	/* _VFSA_IMPL_HPP */

