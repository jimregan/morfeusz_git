/* 
 * File:   Qualifiers.hpp
 * Author: lennyn
 *
 * Created on April 4, 2014, 6:19 PM
 */

#ifndef QUALIFIERS_HPP
#define	QUALIFIERS_HPP

#include <vector>
#include <string>
#include <stdint.h>

/**
 * Helper class used for decoding qualifiers set number into a vector of strings.
 * 
 * @param ptr
 */
class Qualifiers {
public:
    explicit Qualifiers(const unsigned char* ptr);
    
    /**
     * Returns vector of qualifiers represented as strings.
     * @param n - the index in qualifiers tab.
     * @return - vector of qualifiers represented as strings.
     */
    const std::vector<std::string>& getQualifiers(int n) const;
    virtual ~Qualifiers();
private:
    std::vector< std::vector<std::string> > qualifiers;
};

#endif	/* QUALIFIERS_HPP */

