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
#include "Morfeusz.hpp"

using namespace std;

void debug(const string& key, const vector<EncodedInterpretation> value) {
    cerr << key << endl;
    for (EncodedInterpretation i: value) {
        cerr << "suffix to cut: " << i.lemma.suffixToCut << endl;
        cerr << "suffix to add: " << i.lemma.suffixToAdd << endl;
        cerr << "tag: " << i.tag << endl;
        cerr << "name: " << i.nameClassifier << endl;
    }
    cerr << "==================" << endl;
}

void debug(const string& key, const TaggedInterpretation& value) {
    cerr << key << '\t' << value.toString() << endl;
}

void doTest(
        const FSA<vector<EncodedInterpretation>>& fsa, 
        const InterpretationsDecoder<TaggedInterpretation>& interpsConverter, 
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
        vector<EncodedInterpretation> value2;
        fsa.tryToRecognize(key.c_str(), value2);
        DEBUG("recognized "+to_string(value2.size()));
        vector<TaggedInterpretation> parsedValues;
        bool found = false;
        for (EncodedInterpretation interp: value2) {
            TaggedInterpretation parsedValue = interpsConverter.getInterpretation(key, interp);
//            parsedValues.push_back(parsedValue);
            debug(key, parsedValue);
            if (lemma == parsedValue.lemma && tag == parsedValue.tag && name == parsedValue.name) {
                DEBUG("RECOGNIZED");
                found = true;
            }
            else {
                DEBUG("not matching "+parsedValue.lemma+ " " + parsedValue.tag + " " + parsedValue.name);
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
    FSA<vector<EncodedInterpretation>>* fsa = FSA<vector<EncodedInterpretation>>::getFSA(fsaData, deserializer);
    DEBUG("DONE read FSA");
    DEBUG("will read tagset");
    Tagset tagset(fsaData);
    TaggedInterpretationsDecoder interpsDecoder(tagset);
    DEBUG("DONE read tagset");
    DEBUG("still alive");
    doTest(*fsa, interpsDecoder, argv[2]);
    delete fsa;
    return 0;
}
