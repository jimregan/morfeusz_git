

#include <vector>
#include "SegrulesFSA.hpp"

using namespace std;

void SegrulesFSA::proceedToNext(
        const unsigned char segnum,
        const SegrulesState& state,
        bool atEndOfWord,
        vector<SegrulesState>& res) const {
    if (&state == &initialState) {
        for (unsigned int j = 0; j < initialTransitions[segnum].size(); j++) {
            const SegrulesState& newState = initialTransitions[segnum][j];
            if ((atEndOfWord && newState.accepting)
                    || (!atEndOfWord && !newState.sink)) {
                res.push_back(newState);
            }
        }
    }
    else {
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
    for (int i = 0; i < transitionsNum; i++) {
        unsigned char segnum = *currPtr;
        res[segnum].push_back(this->transition2State(currPtr));
        currPtr += 4;
    }
    return res;
}
