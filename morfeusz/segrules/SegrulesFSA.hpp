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

    void proceedToNext(
            const unsigned char segnum,
            const SegrulesState& state,
            bool atEndOfWord,
            std::vector<SegrulesState>& res) const;

    virtual ~SegrulesFSA() {
    }

    SegrulesState initialState;
private:
    const unsigned char* ptr;

    SegrulesState transition2State(const unsigned char* transitionPtr) const;
};

#endif	/* SEGRULESFSA_HPP */

