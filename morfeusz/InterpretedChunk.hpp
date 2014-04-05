/* 
 * File:   InterpretedChunk.hpp
 * Author: mlenart
 *
 * Created on 18 listopad 2013, 15:00
 */

#ifndef INTERPRETEDCHUNK_HPP
#define	INTERPRETEDCHUNK_HPP

#include <vector>
#include "InterpsGroup.hpp"

struct InterpretedChunk {
    unsigned char segmentType;
    const char* textStartPtr;
    const char* textEndPtr;
    std::vector<uint32_t> originalCodepoints;
    std::vector<uint32_t> lowercaseCodepoints;
    const unsigned char* interpsPtr;
    const unsigned char* interpsEndPtr;
    bool shiftOrth;
    bool orthWasShifted;
    std::vector<InterpretedChunk> prefixChunks;
    std::string requiredHomonymId;
};

#endif	/* INTERPRETEDCHUNK_HPP */

