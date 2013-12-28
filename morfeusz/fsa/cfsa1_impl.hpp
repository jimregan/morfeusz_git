/* 
 * File:   cfsa1_impl.hpp
 * Author: mlenart
 *
 * Created on November 4, 2013, 1:08 PM
 */

#ifndef CFSA1_IMPL_HPP
#define	CFSA1_IMPL_HPP

#include <vector>

#include "fsa.hpp"

using namespace std;

#pragma pack(push, 1)  /* push current alignment to stack */

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

static const unsigned int INITIAL_STATE_OFFSET = 257;

template <class T>
vector<unsigned char> CompressedFSA1<T>::initializeChar2PopularCharIdx(const unsigned char* ptr) {
    return vector<unsigned char>(ptr, ptr + INITIAL_STATE_OFFSET);
}

template <class T>
CompressedFSA1<T>::CompressedFSA1(const unsigned char* ptr, const Deserializer<T>& deserializer)
: FSA<T>(ptr + INITIAL_STATE_OFFSET, deserializer),
label2ShortLabel(initializeChar2PopularCharIdx(ptr)) {
}

template <class T>
CompressedFSA1<T>::~CompressedFSA1() {

}

template <class T>
void CompressedFSA1<T>::reallyDoProceed(
        const unsigned char* statePtr,
        State<T>& state) const {
//    const unsigned char stateByte = *statePtr;
    const StateData2* sd = reinterpret_cast<const StateData2*>(statePtr);
    if (sd->accepting) {
//                                            cerr << "ACCEPTING" << endl;
        T object;
        long size = this->deserializer.deserialize(statePtr + 1, object);
        state.setNext(statePtr - this->initialStatePtr, object, size);
    }
    else {
        state.setNext(statePtr - this->initialStatePtr);
    }
}

template <class T>
void CompressedFSA1<T>::doProceedToNextByList(
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
        td = *(reinterpret_cast<const TransitionData2*>(currPtr));
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
    } 
    else {
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
void CompressedFSA1<T>::doProceedToNextByArray(
        const unsigned char shortLabel,
        const uint32_t* ptr,
        State<T>& state) const {
    uint32_t offset = ntohl(ptr[shortLabel]);
    if (offset != 0) {
        const unsigned char* currPtr = this->initialStatePtr + offset;
        reallyDoProceed(currPtr, state);
    }
    else {
        state.setNextAsSink();
    }
}

template <class T>
void CompressedFSA1<T>::proceedToNext(const char c, State<T>& state) const {
//                    if (c <= 'z' && 'a' <= c)
//                        cerr << "NEXT " << c << " from " << state.getOffset() << endl;
//                    else
//                        cerr << "NEXT " << (short) c << " from " << state.getOffset() << endl;
    const unsigned char* fromPointer = this->initialStatePtr + state.getOffset();
    unsigned char shortLabel = this->label2ShortLabel[(const unsigned char) c];
    unsigned long transitionsTableOffset = 1;
    if (state.isAccepting()) {
        transitionsTableOffset += state.getValueSize();
//                                cerr << "transitionsTableOffset " << transitionsTableOffset + state.getOffset() << " because value is " << state.getValue() << endl;
    }
    const StateData2* sd = reinterpret_cast<const StateData2*>(fromPointer);
//                cerr << "transitions num=" << sd->transitionsNum << endl;
    if (sd->array) {
        if (shortLabel > 0) {
            this->doProceedToNextByArray(
                    shortLabel,
                    reinterpret_cast<const uint32_t*>(fromPointer + transitionsTableOffset),
                    state);
        }
        else {
            reallyDoProceed(fromPointer + transitionsTableOffset + 256, state);
            proceedToNext(c, state);
        }
    } 
    else {
        this->doProceedToNextByList(
                c,
                shortLabel,
                fromPointer + transitionsTableOffset,
                sd->transitionsNum,
                state);
    }
}

#endif	/* CFSA1_IMPL_HPP */

