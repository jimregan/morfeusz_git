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

//static bool compareTransitions(TransitionData t1, TransitionData t2) {
//    return t1.label < t2.label;
//}

template <class T>
SimpleFSA<T>::SimpleFSA(const unsigned char* ptr, const Deserializer<T>& deserializer)
: FSA<T>(ptr, deserializer) {
}

template <class T>
SimpleFSA<T>::~SimpleFSA() {

}

//static void debugState(const StateData* stateData) {
//    cerr << "STATE" << endl;
//    cerr << stateData->transitionsNum << " " << stateData->accepting << endl;
//}
//
//static void debugTransitions(const TransitionData* transitionsTable, const TransitionData* transitionsEnd) {
//    int offset = 0;
//    cerr << "TRANSITIONS" << endl;
//    while (transitionsTable + offset < transitionsEnd) {
//        const TransitionData td = *(transitionsTable + offset);
//        if ((td.label <= 'z' && 'a' <= td.label))
//            cerr << td.label << " " << td.targetOffset << endl;
//        else {
//            cerr << ((int) td.label) << " " << td.targetOffset << endl;
//        }
//        offset++;
//    }
//}

template <class T>
void SimpleFSA<T>::proceedToNext(const char c, State<T>& state) const {
//    if (c <= 'z' && 'a' <= c)
//        cerr << "NEXT " << c << " from " << state.getOffset() << endl;
//    else
//        cerr << "NEXT " << (short) c << " from " << state.getOffset() << endl;
//    cerr << "NEXT" << endl;
    const unsigned char* fromPointer = this->startPtr + state.getOffset();
    int transitionsTableOffset = sizeof (StateData);
    if (state.isAccepting()) {
        transitionsTableOffset += state.getValueSize();
//        cerr << "transitionsTableOffset " << transitionsTableOffset + state.getOffset() << " because value is " << state.getValue() << endl;
    }
    StateData stateData = *(StateData*) (fromPointer);
    TransitionData* foundTransition = (TransitionData*) (fromPointer + transitionsTableOffset);
    bool found = false;
    for (int i = 0; i < stateData.transitionsNum; i++, foundTransition++) {
//        cerr << foundTransition->label << endl;
        if (foundTransition->label == c) {
            found = true;
            break;
        }
    }
//    const_cast<Counter*>(&counter)->increment(foundTransition - transitionsStart + 1);
    if (!found) {
//        cerr << "SINK" << (foundTransition == transitionsEnd) << " " << foundTransition->label << " for " << c << endl;
        state.setNextAsSink();
    }
    else {
        //        cerr << "FOUND " << foundTransition->label << " " << foundTransition->targetOffset << endl;
        const unsigned char* nextStatePointer = this->startPtr + foundTransition->targetOffset;
        StateData* nextStateData = (StateData*) (nextStatePointer);
        if (nextStateData->accepting) {
//            cerr << "ACCEPTING" << endl;
            T object;
            int size = this->deserializer.deserialize(nextStatePointer + sizeof (StateData), object);
            state.setNext(foundTransition->targetOffset, object, size);
        } else {
            state.setNext(foundTransition->targetOffset);
        }
    }
}

template <class T>
bool FSA<T>::tryToRecognize(const char* input, T& value) const {
    State<T> currState = this->getInitialState();
    int i = 0;
    while (!currState.isSink() && input[i] != '\0') {
        currState.proceedToNext(input[i]);
        i++;
    }
    // input[i] == '\0'
    currState.proceedToNext(0);
    
    if (currState.isAccepting()) {
        value = currState.getValue();
//        cerr << "RECOGNIZED " << input << endl;
        return true;
    } else {
        return false;
    }
}

template <class T>
State<T> FSA<T>::getInitialState() const {
    return State<T>(*this);
}

#endif	/* _SIMPLE_FSA_IMPL_HPP */
