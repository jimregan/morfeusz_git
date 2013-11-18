/* 
 * File:   InterpretedChunk.hpp
 * Author: mlenart
 *
 * Created on 18 listopad 2013, 15:00
 */

#ifndef INTERPRETEDCHUNK_HPP
#define	INTERPRETEDCHUNK_HPP

#include "InterpsGroup.hpp"

struct InterpretedChunk {
    const char* chunk;
    long chunkLength;
    InterpsGroup interpsGroup;
};

#endif	/* INTERPRETEDCHUNK_HPP */

