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
    unsigned char type;
    uint16_t size;
    const unsigned char* ptr;
};

#endif	/* GROUPEDINTERPRETATIONS_HPP */

