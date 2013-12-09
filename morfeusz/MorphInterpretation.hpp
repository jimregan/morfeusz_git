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
#include "charset/CharsetConverter.hpp"

class MorphInterpretation {
public:
    MorphInterpretation(
            int startNode,
            int endNode,
            const std::string& orth,
            const std::string& lemma,
            int tagnum,
            int namenum,
//            const EncodedInterpretation& encodedInterp,
            const Tagset& tagset,
            const CharsetConverter& charsetConverter);
    static MorphInterpretation createIgn(int startNode, const std::string& orth, const Tagset& tagset, const CharsetConverter& charsetConverter);
//    virtual ~MorphInterpretation();
    int getStartNode() const;
    int getEndNode() const;
    const std::string& getOrth() const;
    const std::string& getLemma() const;
    int getTagnum() const;
    int getNamenum() const;
    const std::string& getTag() const;
    const std::string& getName() const;
private:
    MorphInterpretation(
            int startNode,
            const std::string& orth,
            const Tagset& tagset,
            const CharsetConverter& charsetConverter);
    int startNode;
    int endNode;
    std::string orth;
    std::string lemma;
    int tagnum;
    int namenum;
    std::string tag;
    std::string name;
};

#endif	/* MORPHINTERPRETATION_HPP */

