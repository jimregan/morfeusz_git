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
    int suffixToCut;
    string suffixToAdd;
};

struct Interpretation {
    Lemma lemma;
    list<int> tag;      // np. subst:sg:nom:m1
    int nameClassifier; // np. "pospolita"
    int qualifier;      // np. "dawne" lub "potoczne"
};

#endif	/* INTERPRETATION_HPP */

