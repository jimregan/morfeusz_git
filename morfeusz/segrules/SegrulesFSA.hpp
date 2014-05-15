/* 
 * File:   SegrulesFSA.hpp
 * Author: mlenart
 *
 * Created on 12 marzec 2014, 17:52
 */

#ifndef SEGRULESFSA_HPP
#define	SEGRULESFSA_HPP

#include <set>
#include <iostream>
#include "../deserializationUtils.hpp"

struct SegrulesState {
    uint16_t offset;
    bool accepting;
    bool weak;
    bool shiftOrthFromPrevious;
    bool sink;
};

inline bool operator<(const SegrulesState& s1, const SegrulesState& s2) {
    return s1.offset < s2.offset;
}

class SegrulesFSA {
public:

    SegrulesFSA(const unsigned char* ptr) : initialState(), ptr(ptr) {
        SegrulesState state = {0, false, false, false, false};
        initialState = state;
    }

    std::vector<SegrulesState> proceedToNext(
            const unsigned char segnum,
            const SegrulesState& state,
            bool atEndOfWord) const {
        std::vector<SegrulesState> res;
        const unsigned char* currPtr = ptr + state.offset + 1;
        const unsigned char transitionsNum = *currPtr++;
        for (int i = 0; i < transitionsNum; i++) {
            if (*currPtr == segnum) {
                SegrulesState newState = this->transition2State(currPtr);
                if ((atEndOfWord && newState.accepting) 
                        || (!atEndOfWord && !newState.sink)) {
                    res.push_back(newState);
                }
            }
            currPtr += 4;
        }
        return res;
    }

    virtual ~SegrulesFSA() {
    }

    SegrulesState initialState;
private:
    const unsigned char* ptr;

    SegrulesState transition2State(const unsigned char* transitionPtr) const {
        unsigned char ACCEPTING_FLAG = 1;
        unsigned char WEAK_FLAG = 2;
        SegrulesState res;
        transitionPtr++;
        res.shiftOrthFromPrevious = *transitionPtr++;
        res.offset = readInt16(transitionPtr);
        res.accepting = *(ptr + res.offset) & ACCEPTING_FLAG;
        res.weak = *(ptr + res.offset) & WEAK_FLAG;
        res.sink = *(ptr + res.offset + 1) == 0;
        return res;
    }
};

#endif	/* SEGRULESFSA_HPP */

