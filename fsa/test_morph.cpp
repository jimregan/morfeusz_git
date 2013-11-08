/* 
 * File:   test_morph.cpp
 * Author: mlenart
 *
 * Created on November 8, 2013, 4:12 PM
 */

#include <cstdlib>
#include <sstream>
#include <iostream>
#include "fsa.hpp"
#include "utils.hpp"

using namespace std;

void debug(const string& key, const vector<Interpretation> value) {
    cerr << key << endl;
    for (Interpretation i: value) {
        cerr << "suffix to cut: " << i.lemma.suffixToCut << endl;
        cerr << "suffix to add: " << i.lemma.suffixToAdd << endl;
        cerr << "tag: " << i.tag << endl;
        cerr << "name: " << i.nameClassifier << endl;
    }
    cerr << "==================" << endl;
}

void doTest(const FSA<vector<Interpretation>>& fsa, const char* fname) {
    ifstream ifs;
    //    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(fname, ios::binary);
    string line;
    while (getline(ifs, line)) {
        vector<string> splitVector(split(line, '\t'));
        string key = splitVector[0];
        vector<Interpretation> value2;
        fsa.tryToRecognize(key.c_str(), value2);
        debug(key, value2);
//        validate(fsa.tryToRecognize(key.c_str(), value2), "Failed to recognize " + key);
    }
    validate(ifs.eof(), "Failed to read the input file to the end");
}

int main(int argc, char** argv) {
    validate(argc == 3, "Must provide exactly two arguments - FSA filename and dictionary filename.");
    const unsigned char* fsaData = readFile(argv[1]);
    MorphDeserializer deserializer;
    FSA<vector<Interpretation>>* fsa = FSA<vector<Interpretation>>::getFSA(fsaData, deserializer);
    doTest(*fsa, argv[2]);
    //    cout << argc << endl;
    delete fsa;
    return 0;
}

