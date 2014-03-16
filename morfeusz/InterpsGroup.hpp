/* 
 * File:   GroupedInterpretations.hpp
 * Author: mlenart
 *
 * Created on November 16, 2013, 7:58 PM
 */

#ifndef INTERPSGROUP_HPP
#define	INTERPSGROUP_HPP

#include <vector>
#include <string>
#include "EncodedInterpretation.hpp"
#include "MorphInterpretation.hpp"
#include "Tagset.hpp"

struct InterpsGroup {
//public:
//    
//    InterpsGroup() {
//        
//    }
//    
//    explicit InterpsGroup(const unsigned char type)
//    : type(type) {
//        
//    }
//    
//    void addInterpretation(const EncodedInterpretation& interp) {
//        interps.push_back(interp);
//    }
    
    unsigned char type;
    uint16_t size;
    const unsigned char* ptr;
//    std::vector<EncodedInterpretation> interps;
};

#endif	/* GROUPEDINTERPRETATIONS_HPP */

