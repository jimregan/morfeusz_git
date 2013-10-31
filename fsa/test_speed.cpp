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
    FSAImpl<char*> fsa(fsaData, deserializer);
    ifstream ifs;
//    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(argv[2], ios::binary);
    char line[65536];
    int recognized = 0;
    int unrecognized = 0;
    while (ifs.getline(line, 65536, '\n')) {
        char* val;
//        cout << line << endl;
        if (fsa.tryToRecognize(line, val)) {
//            printf("%s: *OK*\n", line);
            recognized++;
        }
        else {
            unrecognized++;
//            printf("%s: NOT FOUND\n", line);
        }
    }
    cout << "recognized: " << recognized << endl;
    cout << "unrecognized: " << unrecognized << endl;
    cout << "total: " << (recognized + unrecognized) << endl;
    cout << "transitions visited: " << fsa.transitionsCount() << endl;
    return 0;
}
