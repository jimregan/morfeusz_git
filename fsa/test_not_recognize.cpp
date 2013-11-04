/* 
 * File:   test_not_recognize.cpp
 * Author: mlenart
 *
 * Created on 31 październik 2013, 11:40
 */

#include <cstdlib>
#include <sstream>
#include "fsa.hpp"
#include "utils.hpp"

using namespace std;

void doTest(const FSA<char*>& fsa, const char* fname) {
    ifstream ifs;
//    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(fname, ios::binary);
    string line;
    while (getline(ifs, line)) {
        
        vector<string> splitVector(split(line, '\t'));
        string key = splitVector[0];
        
        cerr << "test " << key << endl;
        
        char* value2;
        validate(!fsa.tryToRecognize(key.c_str(), value2), "Recognized "+key+" but should not");
    }
//    validate(ifs.good(), "Something wrong with the input file");
//    validate(!ifs.fail(), "Something wrong with the input file");
    validate(ifs.eof(), "Failed to read the input file to the end");
}

int main(int argc, char** argv) {
    cerr << (int) ((unsigned char) -123) << endl;
    validate(argc == 3, "Must provide exactly two arguments - FSA filename and dictionary filename.");
    const unsigned char* fsaData = readFile(argv[1]);
    StringDeserializer deserializer;
    FSA<char*>* fsa = FSA<char*>::getFSA(fsaData, deserializer);
    doTest(*fsa, argv[2]);
//    cout << argc << endl;
    delete fsa;
    return 0;
}
