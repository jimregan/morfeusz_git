/* 
 * File:   EncodedGeneratorInterpretation.hpp
 * Author: mlenart
 *
 * Created on 20 styczeń 2014, 17:15
 */

#ifndef ENCODEDGENERATORINTERPRETATION_HPP
#define	ENCODEDGENERATORINTERPRETATION_HPP

/*
 * Orth in a compressed format (as in an automaton)
 */
struct EncodedOrth {
    int suffixToCut;
    std::string suffixToAdd;
    std::string prefixToAdd;
};

/*
 * Internal representation of an interpretation - with orth encoded
 */
struct EncodedGeneratorInterpretation {
    EncodedOrth orth;
    int tag;
    int nameClassifier;
};

#endif	/* ENCODEDGENERATORINTERPRETATION_HPP */

