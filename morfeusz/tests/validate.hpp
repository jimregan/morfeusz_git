/* 
 * File:   validate.hpp
 * Author: lennyn
 *
 * Created on June 25, 2014, 10:11 PM
 */

#ifndef VALIDATE_HPP
#define	VALIDATE_HPP

#include <string>
#include <iostream>

template <typename T>
inline void validateEquals(const T required, const T got) {
    bool ok = required == got;
    if (!ok) {
        std::cerr << "ERROR: '" << required << "' != '" << got << "'" << std::endl;
        throw "FAILED";
    }
}

#endif	/* VALIDATE_HPP */

