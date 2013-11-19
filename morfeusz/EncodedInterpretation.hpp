/* 
 * File:   interpretation.hpp
 * Author: mlenart
 *
 * Created on November 4, 2013, 3:11 PM
 */

#ifndef INTERPRETATION_HPP
#define	INTERPRETATION_HPP

#include <string>
#include <sstream>
#include <iterator>
#include "Tagset.hpp"

using namespace std;

/*
 * Lemma in a compressed format (as in an automaton)
 */
struct EncodedLemma {
    int suffixToCut;
    string suffixToAdd;
};

/*
 * Internal representation of an interpretation - with lemma encoded
 */
struct EncodedInterpretation {
    EncodedLemma lemma;
    int type;
    int tag;
    int nameClassifier;
};

#endif	/* INTERPRETATION_HPP */
