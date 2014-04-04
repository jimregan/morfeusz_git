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

class Qualifiers {
public:
    explicit Qualifiers(const unsigned char* ptr);
    std::vector<std::string> getQualifiers(uint16_t n) const;
    virtual ~Qualifiers();
private:
    std::vector< std::vector<std::string> > qualifiers;
};

#endif	/* QUALIFIERS_HPP */

