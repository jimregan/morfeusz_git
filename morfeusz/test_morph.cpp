/* 
 * File:   test_morph.cpp
 * Author: mlenart
 *
 * Created on November 8, 2013, 4:12 PM
 */

//#include <cstdlib>
#include <sstream>
#include <iostream>
#include "fsa.hpp"
#include "interpretations.hpp"
#include "utils.hpp"
#include "MorphDeserializer.hpp"

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

void debug(const string& key, const StringInterpretation& value) {
    cerr << key << '\t' << value.toString() << endl;
}

void doTest(
        const FSA<vector<Interpretation>>& fsa, 
        const InterpretationsConverter& interpsConverter, 
        const char* fname) {
    ifstream ifs;
    //    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(fname, ios::binary);
    string line;
    while (getline(ifs, line)) {
        vector<string> splitVector(split(line, '\t'));
        string key = splitVector[0];
        string lemma = splitVector[1];
        string tag = splitVector[2];
        string name = splitVector[3];
        vector<Interpretation> value2;
        fsa.tryToRecognize(key.c_str(), value2);
        vector<StringInterpretation> parsedValues;
        bool found = false;
        for (Interpretation interp: value2) {
            StringInterpretation parsedValue = interpsConverter.convertInterpretation(key, interp);
//            parsedValues.push_back(parsedValue);
            debug(key, parsedValue);
            if (lemma == parsedValue.lemma && tag == parsedValue.tag && name == parsedValue.name) {
                found = true;
            }
        }
        validate(found, "Failed to recognize " + key + " " + lemma + ":" + tag + ":" + name);
//        debug(key, value2);
//        validate(fsa.tryToRecognize(key.c_str(), value2), "Failed to recognize " + key);
    }
    validate(ifs.eof(), "Failed to read the input file to the end");
}

int main(int argc, char** argv) {
    DEBUG("start test");
    validate(argc == 3, "Must provide exactly two arguments - FSA filename, and dictionary filename.");
    const unsigned char* fsaData = readFile(argv[1]);
    MorphDeserializer deserializer;
    DEBUG("will read FSA");
    FSA<vector<Interpretation>>* fsa = FSA<vector<Interpretation>>::getFSA(fsaData, deserializer);
    DEBUG("DONE read FSA");
    DEBUG("will read tagset");
    InterpretationsConverter converter(fsaData);
    DEBUG("DONE read tagset");
    DEBUG("still alive");
    doTest(*fsa, converter, argv[2]);
    //    cout << argc << endl;
    delete fsa;
    return 0;
}
