/* 
 * File:   test_speed.cpp
 * Author: mlenart
 *
 * Created on 24 październik 2013, 17:47
 */

#include <cstdlib>
#include <fstream>
#include "fsa.hpp"
#include "utils.hpp"

#define NDEBUG

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    validate(argc == 3, "Must provide exactly two arguments - FSA filename and test data filename.");
    const unsigned char* fsaData = readFile(argv[1]);
    StringDeserializer deserializer;
    SimpleFSA<char*> fsa(fsaData, deserializer);
    ifstream ifs;
//    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(argv[2], ios::binary);
    string line;
    while (getline(ifs, line)) {
        char* val;
//        cout << line << endl;
        if (fsa.tryToRecognize(line.c_str(), val)) {
//            printf("%s: *OK*\n", line.c_str());
        }
        else {
//            printf("%s: NOT FOUND\n", line.c_str());
        }
    }
    return 0;
}

