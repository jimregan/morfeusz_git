/* 
 * File:   interpretation.hpp
 * Author: mlenart
 *
 * Created on November 4, 2013, 3:11 PM
 */

#ifndef INTERPRETATION_HPP
#define	INTERPRETATION_HPP

#include <string>
#include <list>

using namespace std;

struct Lemma {
    unsigned short suffixToCut;
    const char* suffixToAdd;
};

struct Interpretation {
    Lemma lemma;
    unsigned int tag;      // np. subst:sg:nom:m1
    unsigned short nameClassifier; // np. "pospolita"
    unsigned short qualifier;      // np. "dawne" lub "potoczne"
};

#endif	/* INTERPRETATION_HPP */

