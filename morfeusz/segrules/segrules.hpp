/* 
 * File:   segrules.hpp
 * Author: mlenart
 *
 * Created on 25 luty 2014, 16:35
 */

#ifndef SEGRULES_HPP
#define	SEGRULES_HPP

#include "../fsa/fsa.hpp"

FSA<unsigned char>* createSegrulesFSA(const unsigned char* analyzerPtr);

#endif	/* SEGRULES_HPP */

