/* 
 * File:   interpretation.hpp
 * Author: mlenart
 *
 * Created on November 4, 2013, 3:11 PM
 */

#ifndef INTERPRETATION_HPP
#define	INTERPRETATION_HPP

#include <string>
#include <vector>
#include <cstdint>
#include "Tagset.hpp"

/*
 * Lemma in a compressed format (as in an automaton)
 */
struct EncodedLemma {
    int suffixToCut;
    std::string suffixToAdd;
    std::vector<bool> casePattern;
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