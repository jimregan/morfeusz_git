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
#include "../deserializationUtils.hpp"

using namespace std;

static const unsigned char CFSA1_ACCEPTING_FLAG = 128;
static const unsigned char CFSA1_ARRAY_FLAG = 64;
static const unsigned char CFSA1_TRANSITIONS_NUM_MASK = 63;

static const unsigned char CFSA1_OFFSET_SIZE_MASK = 3;

static const unsigned int CFSA1_INITIAL_ARRAY_STATE_OFFSET = 257;

struct StateData2 {
    unsigned int transitionsNum;
    bool isArray;
    bool isAccepting;
};

struct TransitionData2 {
    unsigned int offsetSize;
    unsigned int shortLabel;
};

static inline StateData2 readStateData(const unsigned char*& ptr) {
    StateData2 res;
    unsigned char firstByte = readInt8(ptr);
    res.isArray = firstByte & CFSA1_ARRAY_FLAG;
    res.isAccepting = firstByte & CFSA1_ACCEPTING_FLAG;
    res.transitionsNum = firstByte & CFSA1_TRANSITIONS_NUM_MASK;
    if (res.transitionsNum == CFSA1_TRANSITIONS_NUM_MASK) {
        res.transitionsNum = readInt8(ptr);
    }
    return res;
}

static inline TransitionData2 readTransitionFirstByte(const unsigned char*& ptr) {
    TransitionData2 res;
    unsigned char firstByte = readInt8(ptr);
    res.offsetSize = firstByte & CFSA1_OFFSET_SIZE_MASK;
    res.shortLabel = firstByte >> 2;
    return res;
}

template <class T>
vector<unsigned char> CompressedFSA1<T>::initializeChar2PopularCharIdx(const unsigned char* ptr) {
    return vector<unsigned char>(ptr, ptr + CFSA1_INITIAL_ARRAY_STATE_OFFSET);
}

template <class T>
CompressedFSA1<T>::CompressedFSA1(const unsigned char* ptr, const Deserializer<T>& deserializer)
: FSA<T>(ptr + CFSA1_INITIAL_ARRAY_STATE_OFFSET, deserializer),
label2ShortLabel(initializeChar2PopularCharIdx(ptr)) {
}

template <class T>
CompressedFSA1<T>::~CompressedFSA1() {

}

template <class T>
void CompressedFSA1<T>::reallyDoProceed(
        const unsigned char* statePtr,
        State<T>& state) const {
    const unsigned char* currPtr = statePtr;
    const StateData2 sd = readStateData(currPtr);
    if (sd.isAccepting) {
        T object;
        long size = this->deserializer.deserialize(currPtr, object);
        //        long size = this->deserializer.deserialize(statePtr + 1, object);
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
    const unsigned char* currPtr = ptr;
    //    TransitionData* foundTransition = (TransitionData*) (fromPointer + transitionsTableOffset);
    bool found = false;
    TransitionData2 td;
    for (unsigned int i = 0; i < transitionsNum; i++) {
        td = readTransitionFirstByte(currPtr);
        if (td.shortLabel == shortLabel) {
            if (shortLabel == 0) {
                char label = static_cast<char>(readInt8(currPtr));
                if (label == c) {
                    found = true;
                    break;
                }
                else {
                    currPtr += td.offsetSize;
                }
            }
            else {
                found = true;
                break;
            }
        }
        else {
            if (td.shortLabel == 0) {
                currPtr++;
            }
            currPtr += td.offsetSize;
        }
    }
    if (!found) {
        state.setNextAsSink();
    }
    else {
        uint32_t offset;
        switch (td.offsetSize) {
            case 0:
                offset = 0;
                break;
            case 1:
                offset = readInt8(currPtr);
                break;
            case 2:
                offset = readInt16(currPtr);
                break;
            case 3:
               offset = readInt16(currPtr);
               offset <<= 8;
               offset += readInt8(currPtr);
               break;
        }
        currPtr += offset;
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
    const unsigned char* currPtr = this->initialStatePtr + state.getOffset();
    unsigned char shortLabel = this->label2ShortLabel[(const unsigned char) c];
    //    unsigned long transitionsTableOffset = 1;
    const StateData2 sd = readStateData(currPtr);
    if (state.isAccepting()) {
        currPtr += state.getValueSize();
    }

    if (sd.isArray) {
        if (shortLabel > 0) {
            this->doProceedToNextByArray(
                    shortLabel,
                    reinterpret_cast<const uint32_t*> (currPtr),
                    state);
        }
        else {
            reallyDoProceed(currPtr + 256, state);
            proceedToNext(c, state);
        }
    }
    else {
        this->doProceedToNextByList(
                c,
                shortLabel,
                currPtr,
                sd.transitionsNum,
                state);
    }
}

#endif	/* CFSA1_IMPL_HPP */

