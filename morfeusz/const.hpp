/* 
 * File:   const.hpp
 * Author: lennyn
 *
 * Created on December 3, 2013, 9:04 PM
 */

#ifndef CONST1_HPP
#define	CONST1_HPP

#include "morfeusz2.h"

namespace morfeusz {

enum MorfeuszProcessorType {
    GENERATOR,
    ANALYZER
};

extern const Charset DEFAULT_MORFEUSZ_CHARSET;

extern const unsigned char SHIFT_ORTH_NODE;

extern const char HOMONYM_SEPARATOR;

}

#endif	/* CONST_HPP */

