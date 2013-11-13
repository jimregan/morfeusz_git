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

struct EncodedLemma {
    int suffixToCut;
    string suffixToAdd;
};

/*
 * Internal representation of an interpretation - with lemma encoded
 */
struct EncodedInterpretation {
    EncodedLemma lemma;
    int tag;
    int nameClassifier;
};

class MorphInterpretation {
public:
    MorphInterpretation(
            int startNode,
            int endNode,
            const std::string& orth,
            const EncodedInterpretation& encodedInterp);
    const std::string& getOrth() const;
    const std::string& getLemma() const;
    int getTagnum() const;
    int getNamenum() const;
    const std::string& getTag(const Tagset& tagset) const;
    const std::string& getName(const Tagset& tagset) const;
private:
    int startNode;
    int endNode;
    std::string orth;
    std::string lemma;
    int tagnum;
    int namenum;
};

// ALL BELOW IS DEPRECATED

/*
 * Interpretation with tags as integers (need a Tagset object to decode them)
 */
struct RawInterpretation {
    string lemma;
    int tagnum;
    int namenum;
};

/*
 * Interpretation with tags as strings (already processed with a Tagset object)
 */
struct TaggedInterpretation {
    std::string lemma;
    const std::string& tag;      // np. subst:sg:nom:m1
    const std::string& name; // np. "pospolita"
    std::string toString() const;
};

template <class InterpType>
class InterpretationsDecoder {
public:
//    explicit InterpretationsDecoder(const Tagset& tagset);
    
    virtual InterpType getInterpretation(
            const std::string& orth, 
            const EncodedInterpretation& interp) const = 0;
    
protected:
    std::string convertLemma(const std::string& orth, const EncodedLemma& interp) const;
};

class TaggedInterpretationsDecoder: public InterpretationsDecoder<TaggedInterpretation> {
public:
    explicit TaggedInterpretationsDecoder(const Tagset& tagset);
    
    TaggedInterpretation getInterpretation(
            const std::string& orth, 
            const EncodedInterpretation& interp) const;
private:
    Tagset tagset;
};

class RawInterpretationsDecoder: public InterpretationsDecoder<RawInterpretation> {
public:
    RawInterpretationsDecoder();
    
    RawInterpretation getInterpretation(
            const std::string& orth, 
            const EncodedInterpretation& interp) const;
};

#endif	/* INTERPRETATION_HPP */

