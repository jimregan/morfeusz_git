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

class InterpsGroup {
public:
    
    InterpsGroup() {
        
    }
    
    explicit InterpsGroup(const int type)
    : type(type) {
        
    }
    
//    std::vector<MorphInterpretation> getRealInterps(
//                const std::string& orth, 
//                const int startNode, 
//                const int endNode,
//                const Tagset& tagset) {
//        std::vector<MorphInterpretation> res;
//        for (EncodedInterpretation& ei: interps) {
//            res.push_back(MorphInterpretation(startNode, endNode, orth, ei, tagset));
//        }
//        return res;
//    }
    
    void addInterpretation(const EncodedInterpretation& interp) {
        interps.push_back(interp);
    }
    
    int type;
    std::vector<EncodedInterpretation> interps;
};

#endif	/* GROUPEDINTERPRETATIONS_HPP */

