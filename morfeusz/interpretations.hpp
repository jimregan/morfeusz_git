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
#include "Tagset.hpp"

using namespace std;

struct Lemma {
    int suffixToCut;
    string suffixToAdd;
};

struct Interpretation {
    Interpretation();
    Interpretation(const Lemma& lemma, const int tag, const int name);
    Lemma lemma;
    int tag;      // np. subst:sg:nom:m1
    int nameClassifier; // np. "pospolita"
//    int qualifier;      // np. "dawne" lub "potoczne"
};

struct StringInterpretation {
    StringInterpretation(const std::string& lemma, const std::string& tag, const std::string& name);
    const std::string lemma;
    const std::string& tag;      // np. subst:sg:nom:m1
    const std::string& name; // np. "pospolita"
//    std::string qualifier;      // np. "dawne" lub "potoczne"
    std::string toString() const;
};

class LemmaConverter {
public:
    std::string convertLemma(const std::string& orth, const Lemma& interp) const;
};


class InterpretationsConverter {
public:
    explicit InterpretationsConverter(const unsigned char* data);
    StringInterpretation convertInterpretation(
            const std::string& orth, 
            const Interpretation& interp) const;
private:
    LemmaConverter lemmaConverter;
    Tagset tagset;
};

#endif	/* INTERPRETATION_HPP */

