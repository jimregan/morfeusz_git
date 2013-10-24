/* 
 * File:   utils.hpp
 * Author: mlenart
 *
 * Created on 24 październik 2013, 17:56
 */

#ifndef UTILS_HPP
#define	UTILS_HPP

#include <iostream>
#include <string>

void validate(const bool cond, const std::string& msg) {
    if (!cond) {
        std::cerr << msg << std::endl;
        exit(1);
    }
}

unsigned char* readFile(const char* fname) {
    ifstream ifs;
    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(fname, ios::in | ios::binary | ios::ate);
    //    if (ifs.is_open()) {
    int size = ifs.tellg();
    unsigned char* memblock = new unsigned char [size];
    ifs.seekg(0, ios::beg);
    ifs.read(reinterpret_cast<char*> (memblock), size);
    ifs.close();
    return memblock;
    //    } 
    //    else {
    //        cerr << "Unable to open file " << fname << endl;
    //    }
}

#endif	/* UTILS_HPP */

