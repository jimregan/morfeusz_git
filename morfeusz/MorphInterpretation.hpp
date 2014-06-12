/* 
 * File:   MorphInterpretation.hpp
 * Author: mlenart
 *
 * Created on November 14, 2013, 11:47 AM
 */

#ifndef MORPHINTERPRETATION_HPP
#define	MORPHINTERPRETATION_HPP

#include <string>

class Environment;

#include "Tagset.hpp"
#include "charset/CharsetConverter.hpp"
#include "Environment.hpp"

/**
 * Morphological interpretation as seen by the user in the analysis/generation results.
 */
class MorphInterpretation {
public:
    MorphInterpretation(
            int startNode,
            int endNode,
            const std::string& orth,
            const std::string& lemma,
//            const std::string& homonymId,
            int tagnum,
            int namenum,
            int qualifiersNum,
            const Environment& env);
    MorphInterpretation();
    static MorphInterpretation createIgn(int startNode, const std::string& orth, const Environment& env);
//    virtual ~MorphInterpretation();
    int getStartNode() const;
    int getEndNode() const;
    const std::string& getOrth() const;
    const std::string& getLemma() const;
//    const std::string& getHomonymId() const;
    bool hasHomonym(const string& homonymId) const;
    int getTagnum() const;
    int getNamenum() const;
    const std::string& getTag() const;
    const std::string& getName() const;
    const std::vector<std::string>& getQualifiers() const;
    
    std::string toString(bool includeNodeNumbers) const;
private:
    MorphInterpretation(
            int startNode,
            const std::string& orth,
            const Environment& env);
    int startNode;
    int endNode;
    std::string orth;
    std::string lemma;
//    std::string homonymId;
    int tagnum;
    int namenum;
    std::string tag;
    std::string name;
    const std::vector<std::string>* qualifiers;
};

#endif	/* MORPHINTERPRETATION_HPP */

