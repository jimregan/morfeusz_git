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
    unsigned transitionsNum : 6;
    unsigned next : 1;
    unsigned accepting : 1;
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
    //    vector<unsigned char> res(256, FSAImpl<bool>::POPULAR_CHARS_NUM);
    //    const unsigned char* popularChars = ptr + getPopularCharsOffset();
    //    for (unsigned int i = 0; i < POPULAR_CHARS_NUM; i++) {
    //        res[popularChars[i]] = i;
    //    }
    //    return res;
    return vector<unsigned char>();
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
    //    cerr << "initial state offset " << getInitialStateOffset() << endl;
}

template <class T>
FSAImpl<T>::~FSAImpl() {

}

template <class T>
void FSAImpl<T>::proceedToNext(const char c, State<T>& state) const {
//            if (c <= 'z' && 'a' <= c)
//                cerr << "NEXT " << c << " from " << state.getOffset() << endl;
//            else
//                cerr << "NEXT " << (short) c << " from " << state.getOffset() << endl;
    const unsigned char* fromPointer = this->startPtr + state.getOffset();
    int transitionsTableOffset = sizeof (StateData2);
    if (state.isAccepting()) {
        transitionsTableOffset += state.getValueSize();
//                cerr << "transitionsTableOffset " << transitionsTableOffset + state.getOffset() << " because value is " << state.getValue() << endl;
    }
    StateData2 stateData = *(StateData2*) (fromPointer);
//        cerr << "transitions num=" << stateData.transitionsNum << endl;
    register unsigned char* currPtr = (unsigned char*) (fromPointer + transitionsTableOffset);
    //    TransitionData* foundTransition = (TransitionData*) (fromPointer + transitionsTableOffset);
    bool found = false;
    bool next = stateData.next;
    for (unsigned int i = 0; i < stateData.transitionsNum; i++) {
//                cerr << *currPtr << endl;      
        if ((char) *currPtr == c) {
            found = true;
            next = next && i + 1 == stateData.transitionsNum;
            break;
        } else {
            //            unsigned int offsetSize = currPtr[1] & 0b00000011;
            currPtr += (currPtr[1] & 0b00000011) + 2;
        }
    }
    //    const_cast<Counter*>(&counter)->increment(foundTransition - transitionsStart + 1);
    if (!found) {
//                cerr << "SINK for " << c << endl;
        state.setNextAsSink();
    }
    else {
        currPtr++;
        if (!next) {
            unsigned int offsetSize = *currPtr & 0b00000011;
            unsigned int offset = *currPtr >> 2;
//                    cerr << "offset size " << offsetSize << endl;
//                    cerr << "offset " << offset << endl;
            currPtr++;
            //        currPtr += (*currPtr >> 2) + 1;
            switch (offsetSize) {
                case 0:
                    currPtr += offset;
                    break;
                case 1:
                    currPtr += (offset << 8) + *currPtr + 1;
                    break;
                case 2:
                    currPtr += (offset << 16) + ntohs(*((uint16_t*) currPtr)) + 2;
                    break;
                case 3:
                    currPtr += (offset << 24) + (((unsigned int) ntohs(*((uint16_t*) currPtr))) << 8) + currPtr[2] + 3;
                    break;
            }
        }
//                cerr << "FOUND " << c << " " << currPtr - this->startPtr << endl;
        StateData* nextStateData = (StateData*) (currPtr);
        if (nextStateData->accepting) {
//                        cerr << "ACCEPTING" << endl;
            T object;
            int size = this->deserializer.deserialize(currPtr + sizeof (StateData), object);
            state.setNext(currPtr - this->startPtr, object, size);
        } else {
            state.setNext(currPtr - this->startPtr);
        }
    }
}

//template <class T>
//void FSAImpl<T>::proceedToNext(const char c, State<T>& state) const {
//    //    if (c <= 'z' && 'a' <= c)
//    //        cerr << "NEXT " << c << " from " << state.getOffset() << endl;
//    //    else
//    //        cerr << "NEXT " << (short) c << " from " << state.getOffset() << endl;
//    const unsigned char* fromPointer = this->startPtr + state.getOffset();
//    unsigned int transitionsTableOffset = 0;
//    if (state.isAccepting()) {
//        transitionsTableOffset += state.getValueSize();
//        //        cerr << "transitionsTableOffset " << transitionsTableOffset + state.getOffset() << " because value is " << state.getValue() << endl;
//    }
//    
//    bool found = false;
////    bool failed = false;
//    unsigned int requiredShortLabel = char2PopularCharIdx[(unsigned char) c];
//    //    cerr << "NEXT " << c << " " << (int) shortLabel << endl;
////    VTransitionData* td;
////    unsigned char transitionByte = *currPtr;
//    unsigned int offsetSize;
//    register const unsigned char* currPtr = fromPointer + transitionsTableOffset;
//    
//    while (!found) {
//        
//        register unsigned char firstByte = *currPtr;
//        
//        unsigned int shortLabel = firstByte & 0b00011111;
//        bool last = (firstByte & 0b10000000);
//        offsetSize = (firstByte & 0b01100000) >> 5;
//
//        const_cast<FSAImpl<T>*>(this)->counter.increment(1);
//        
//        if (shortLabel != requiredShortLabel) {
//            if (last) {
//                break;
//            }
//            currPtr += offsetSize + 1;
//            if (shortLabel == POPULAR_CHARS_NUM) {
//                currPtr++;
//            }
//        }
//        else if (shortLabel != POPULAR_CHARS_NUM) {
//            found = true;
//            currPtr++;
//        }
//        else {
//            currPtr++;
//            char realLabel = (char) *currPtr;
//            if (realLabel != c) {
//                if (last) {
//                    break;
//                }
//                currPtr += offsetSize + 1;
//            }
//            else {
//                found = true;
//                currPtr++;
//            }
//        }
//    }
//
//    if (found) {
//        switch (offsetSize) {
//            case 0:
//                break;
//            case 1:
//                currPtr += *currPtr + 1;
//                break;
//            case 2:
//                currPtr += ntohs(*((uint16_t*) currPtr)) + 2;
//                break;
//            case 3:
//                currPtr += (((unsigned int) ntohs(*((uint16_t*) currPtr))) << 8) + currPtr[2] + 3;
//                break;
//        }
//        bool accepting = c == '\0';
//        if (accepting) {
//            T value;
//            int valueSize = this->deserializer.deserialize(currPtr, value);
//            currPtr += valueSize;
//            state.setNext(currPtr - this->startPtr, value, valueSize);
//        }
//        else {
//            state.setNext(currPtr - this->startPtr);
//        }
//    }
//    else {
//        state.setNextAsSink();
//    }
//}

#endif	/* _VFSA_IMPL_HPP */

