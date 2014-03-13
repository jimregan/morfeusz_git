/* 
 * File:   SegrulesFSA.hpp
 * Author: mlenart
 *
 * Created on 12 marzec 2014, 17:52
 */

#ifndef SEGRULESFSA_HPP
#define	SEGRULESFSA_HPP

#include <set>
#include "../endianness.hpp"

struct SegrulesState {
    uint16_t offset;
    bool accepting;
    bool weak;
    bool shiftOrthFromPrevious;
};

inline bool operator<(const SegrulesState& s1, const SegrulesState& s2)
{
  return s1.offset < s2.offset;
}

class SegrulesFSA {
public:
    SegrulesFSA(const unsigned char* ptr): initialState(), ptr(ptr) {
        SegrulesState state = {0, false, false, false};
        initialState = state;
    }
    
    void proceedToNext(
        const unsigned char segnum,
        const SegrulesState state,
        std::set<SegrulesState>& newStates) const {
        
        const unsigned char* currPtr = ptr + state.offset;
        currPtr++;
        const unsigned char transitionsNum = *currPtr;
        currPtr++;
        for (unsigned int i = 0; i < transitionsNum; i++) {
            if (*currPtr == segnum) {
                newStates.insert(newStates.begin(), this->transition2State(currPtr));
            }
            currPtr += 4;
        }
    }
    
    virtual ~SegrulesFSA() {}
    
    SegrulesState initialState;
private:
    const unsigned char* ptr;
    
    SegrulesState transition2State(const unsigned char* transitionPtr) const {
        unsigned char ACCEPTING_FLAG = 1;
        unsigned char WEAK_FLAG = 2;
        SegrulesState res;
        transitionPtr++;
        res.shiftOrthFromPrevious = *transitionPtr;
        transitionPtr++;
        res.offset = htons(*reinterpret_cast<const uint16_t*>(transitionPtr));
        res.accepting = *(ptr + res.offset) & ACCEPTING_FLAG;
        res.weak = *(ptr + res.offset) & WEAK_FLAG;
        return res;
    }
};

#endif	/* SEGRULESFSA_HPP */

