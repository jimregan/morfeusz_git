/* 
 * File:   MorphInterpretation.hpp
 * Author: mlenart
 *
 * Created on November 14, 2013, 11:47 AM
 */

#ifndef MORPHINTERPRETATION_HPP
#define	MORPHINTERPRETATION_HPP

#include <string>
#include "Tagset.hpp"
#include "EncodedInterpretation.hpp"

class MorphInterpretation {
public:
    MorphInterpretation(
            int startNode,
            int endNode,
            const std::string& orth,
            const EncodedInterpretation& encodedInterp,
            const Tagset& tagset);
    virtual ~MorphInterpretation();
    const std::string& getOrth() const;
    const std::string& getLemma() const;
    int getTagnum() const;
    int getNamenum() const;
    const std::string& getTag() const;
    const std::string& getName() const;
private:
    int startNode;
    int endNode;
    std::string orth;
    std::string lemma;
    int tagnum;
    int namenum;
    const std::string& tag;
    const std::string& name;
};

#endif	/* MORPHINTERPRETATION_HPP */

