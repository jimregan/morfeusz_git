/* 
 * File:   segrules.hpp
 * Author: mlenart
 *
 * Created on 25 luty 2014, 16:35
 */

#ifndef SEGRULES_HPP
#define	SEGRULES_HPP

#include <utility>
#include <map>
#include <string>
#include "../fsa/fsa.hpp"

typedef std::map<std::string, std::string> SegrulesOptions;
typedef State<unsigned char> SegrulesStateType;
typedef FSA<unsigned char> SegrulesFSAType;

std::map<SegrulesOptions, SegrulesFSAType*> createSegrulesFSAsMap(const unsigned char* analyzerPtr);

#endif	/* SEGRULES_HPP */

