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
    const char* chunkStartPtr;
    const char* chunkEndPtr;
    std::vector<uint32_t> originalCodepoints;
    std::vector<uint32_t> lowercaseCodepoints;
    InterpsGroup interpsGroup;
    bool shiftOrth;
    bool orthWasShifted;
    std::vector<InterpretedChunk> prefixChunks;
    std::string requiredHomonymId;
};

#endif	/* INTERPRETEDCHUNK_HPP */

