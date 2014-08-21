

#include <vector>
#include <cassert>
#include "SegrulesFSA.hpp"

using namespace std;

namespace morfeusz {

void SegrulesFSA::proceedToNext(
        const unsigned char segnum,
        const SegrulesState& state,
        bool atEndOfWord,
        vector<SegrulesState>& res) const {
    if (state.offset == 0) {
        doProceedFromInitialState(segnum, atEndOfWord, res);
    }
    else {
        doProceedFromNonInitialState(segnum, state, atEndOfWord, res);
    }
}

void SegrulesFSA::doProceedFromInitialState(
        const unsigned char segnum,
        bool atEndOfWord,
        vector<SegrulesState>& res) const {
    const vector<SegrulesState>& newStates = initialTransitions[segnum];
    vector<SegrulesState>::const_iterator it = newStates.begin();
    while (it != newStates.end()) {
        const SegrulesState& newState = *it++;
        if ((atEndOfWord && newState.accepting)
                || (!atEndOfWord && !newState.sink)) {
            res.push_back(newState);
        }
    }
}

void SegrulesFSA::doProceedFromNonInitialState(
        const unsigned char segnum,
        const SegrulesState& state,
        bool atEndOfWord,
        std::vector<SegrulesState>& res) const {
    const unsigned char* currPtr = ptr + state.offset + 1;
    const unsigned char transitionsNum = *currPtr++;
    assert(transitionsNum <= 1);
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
}

SegrulesState SegrulesFSA::transition2State(const unsigned char* transitionPtr) const {
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

vector< vector<SegrulesState> > SegrulesFSA::createInitialTransitionsVector() {
    vector< vector<SegrulesState> > res(256, vector<SegrulesState>());
    const unsigned char* currPtr = ptr + initialState.offset + 1;
    const unsigned char transitionsNum = *currPtr++;
    assert(transitionsNum <= 1);
    for (int i = 0; i < transitionsNum; i++) {
        unsigned char segnum = *currPtr;
        res[segnum].push_back(this->transition2State(currPtr));
        currPtr += 4;
    }
    return res;
}

}
