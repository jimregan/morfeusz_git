/* 
 * File:   MorfeuszOptions.hpp
 * Author: mlenart
 *
 * Created on December 7, 2013, 9:36 PM
 */

#ifndef MORFEUSZOPTIONS_HPP
#define	MORFEUSZOPTIONS_HPP

#include "const.hpp"

namespace morfeusz {

/**
 * Represents options for Morfeusz analyzer/generator.
 */
struct MorfeuszOptions {
    bool caseSensitive;
    Charset encoding;
    bool debug;
};

}

#endif	/* MORFEUSZOPTIONS_HPP */

