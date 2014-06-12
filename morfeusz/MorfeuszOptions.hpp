/* 
 * File:   MorfeuszOptions.hpp
 * Author: mlenart
 *
 * Created on December 7, 2013, 9:36 PM
 */

#ifndef MORFEUSZOPTIONS_HPP
#define	MORFEUSZOPTIONS_HPP

#include "const.hpp"

/**
 * Represents options for Morfeusz analyzer/generator.
 */
struct MorfeuszOptions {
    bool caseSensitive;
    MorfeuszCharset encoding;
    bool debug;
};

#endif	/* MORFEUSZOPTIONS_HPP */

