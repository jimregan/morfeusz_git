/* 
 * File:   const.hpp
 * Author: lennyn
 *
 * Created on December 3, 2013, 9:04 PM
 */

#ifndef CONST1_HPP
#define	CONST1_HPP

namespace morfeusz {

enum MorfeuszCharset {
    UTF8,
//    UTF16LE,
//    UTF16BE,
//    UTF32,
    ISO8859_2,
    CP1250,
    CP852
};

enum MorfeuszProcessorType {
    GENERATOR,
    ANALYZER
};

extern const MorfeuszCharset DEFAULT_MORFEUSZ_CHARSET;

extern const unsigned char SHIFT_ORTH_NODE;

extern const char HOMONYM_SEPARATOR;

}

#endif	/* CONST_HPP */

