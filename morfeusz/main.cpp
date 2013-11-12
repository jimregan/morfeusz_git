/* 
 * File:   main.cc
 * Author: mlenart
 *
 * Created on October 8, 2013, 12:41 PM
 */

#include <cstdlib>
#include <iostream>
#include "fsa.hpp"
#include "default_fsa.hpp"
#include "Tagset.hpp"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    unsigned char dupa[3] = {0376 | 1, 0111, 0234, };
    char x = 255;
    cout << *reinterpret_cast<int*>(&x) << endl;
    return 0;
}


