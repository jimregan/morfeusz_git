/* 
 * File:   _simple_fsa_impl.hpp
 * Author: mlenart
 *
 * Created on October 20, 2013, 12:25 PM
 */

#ifndef _SIMPLE_FSA_IMPL_HPP
#define	_SIMPLE_FSA_IMPL_HPP

#include <algorithm>
#include <utility>
#include <iostream>
#include "fsa.hpp"

using namespace std;

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

struct StateData {
    unsigned transitionsNum : 7;
    unsigned accepting : 1;
};

struct TransitionData {
    char label;
    unsigned targetOffset : 24;
};

#pragma pack(pop)   /* restore original alignment from stack */

static bool compareTransitions(TransitionData t1, TransitionData t2) {
    return t1.label < t2.label;
}

template <class T>
SimpleFSA<T>::SimpleFSA(const unsigned char* ptr, const Deserializer<T>& deserializer)
: FSA<T>(ptr, deserializer) {
}

template <class T>
SimpleFSA<T>::~SimpleFSA() {

}

static void debugState(const StateData* stateData) {
    cerr << "STATE" << endl;
    cerr << stateData->transitionsNum << " " << stateData->accepting << endl;
}

static void debugTransitions(const TransitionData* transitionsTable, const TransitionData* transitionsEnd) {
    int offset = 0;
    cerr << "TRANSITIONS" << endl;
    while (transitionsTable + offset < transitionsEnd) {
        const TransitionData td = *(transitionsTable + offset);
        if ((td.label <= 'z' && 'a' <= td.label))
        cerr <<  td.label << " " << td.targetOffset << endl;
        else {
            cerr <<  ((int) td.label) << " " << td.targetOffset << endl;
        }
        offset++;
    }
}

template <class T>
void SimpleFSA<T>::proceedToNext(const char c, State<T>& state) const {
    if (c<= 'z' && 'a' <= c)
    cerr << "NEXT " << c << " from " << state.getOffset() << endl;
    else
        cerr << "NEXT " << (short) c << " from " << state.getOffset() << endl;
    const unsigned char* fromPointer = this->startPtr + state.getOffset();
    int transitionsTableOffset = sizeof (StateData);
    if (state.isAccepting()) {
        transitionsTableOffset += state.getValueSize();
        cerr << "transitionsTableOffset " << transitionsTableOffset + state.getOffset() << " because value is " << state.getValue() << endl;
    }
    const StateData* stateData = reinterpret_cast<const StateData*> (fromPointer);
    const TransitionData* transitionsTable = reinterpret_cast<const TransitionData*> (fromPointer + transitionsTableOffset);
    const TransitionData* transitionsEnd = transitionsTable + stateData->transitionsNum;
    debugState(stateData);
    debugTransitions(transitionsTable, transitionsEnd);
    const TransitionData* foundTransition = std::lower_bound(
            transitionsTable, transitionsEnd,
            TransitionData{c, 0},
            compareTransitions);
    if (foundTransition == transitionsEnd || foundTransition->label != c) {
        cerr << "SINK" << (foundTransition == transitionsEnd) << " " << foundTransition->label << " for " << c << endl;
        state.setNextAsSink();
    }
    else {
//        cerr << "FOUND " << foundTransition->label << " " << foundTransition->targetOffset << endl;
        const unsigned char* nextStatePointer = this->startPtr + foundTransition->targetOffset;
        const StateData* nextStateData = reinterpret_cast<const StateData*> (nextStatePointer);
        if (nextStateData->accepting) {
            cerr << "ACCEPTING" << endl;
            T object;
            int size = this->deserializer.deserialize(nextStatePointer + sizeof (StateData), object);
            state.setNext(foundTransition->targetOffset, object, size);
        }
        else {
            state.setNext(foundTransition->targetOffset);
        }
    }
}

#endif	/* _SIMPLE_FSA_IMPL_HPP */
