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
    const unsigned char* interpsGroupPtr;
    const unsigned char* interpsPtr;
    const unsigned char* interpsEndPtr;
    bool shiftOrth;
    bool orthWasShifted;
    int codepointsNum;
    std::vector<InterpretedChunk> prefixChunks;
    std::string requiredHomonymId;
};

#endif	/* INTERPRETEDCHUNK_HPP */

