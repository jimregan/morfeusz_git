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
#include <inttypes.h>
#include "Tagset.hpp"

/*
 * Lemma in a compressed format (as in an automaton)
 */
struct EncodedForm {
    int suffixToCut;
    std::string suffixToAdd;
    std::vector<bool> casePattern;
    std::string prefixToAdd;
};

/*
 * Internal representation of an interpretation - with lemma encoded
 */
struct EncodedInterpretation {
    EncodedForm value;
    int tag;
    int nameClassifier;
    std::string homonymId;
};

#endif	/* INTERPRETATION_HPP */
