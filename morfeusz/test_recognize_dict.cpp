/* 
 * File:   test_morph.cpp
 * Author: mlenart
 *
 * Created on November 8, 2013, 4:12 PM
 */

//#include <cstdlib>
#include <sstream>
#include <iostream>
#include "EncodedInterpretation.hpp"
#include "utils.hpp"
#include "MorphDeserializer.hpp"
#include "Morfeusz.hpp"
#include "MorphInterpretation.hpp"

using namespace std;

//void doTest(
//        const FSA<vector<InterpsGroup >> &fsa,
//        const Tagset& tagset,
//        //        const InterpretationsDecoder<TaggedInterpretation>& interpsConverter, 
//        const char* fname) {
//    ifstream ifs;
//    //    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//    ifs.open(fname, ios::binary);
//    string line;
//    while (getline(ifs, line)) {
//        vector<string> splitVector(split(line, '\t'));
//        string orth = splitVector[0];
//        string lemma = splitVector[1];
//        string tag = splitVector[2];
//        string name = splitVector[3];
//        vector<InterpsGroup> value2;
//        fsa.tryToRecognize(orth.c_str(), value2);
//        DEBUG("recognized " + to_string(value2.size()));
//        //        vector<TaggedInterpretation> parsedValues;
//        bool found = false;
//
//        for (InterpsGroup ig : value2)
//            for (MorphInterpretation interp : ig.getRealInterps(orth, 0, 0, tagset)) {
//                //            TaggedInterpretation parsedValue = interpsConverter.getInterpretation(key, interp);
//                //            (0, 0, orth, encodedInterp, tagset);
//                //            parsedValues.push_back(parsedValue);
//                //            debug(orth, parsedValue);
//                if (lemma == interp.getLemma() && tag == interp.getTag() && name == interp.getName()) {
//                    DEBUG("RECOGNIZED");
//                    found = true;
//                }
//                else {
//                    DEBUG("not matching " + interp.getLemma() + " " + interp.getTag() + " " + interp.getName());
//                }
//            }
//        validate(found, "Failed to recognize " + orth + " " + lemma + ":" + tag + ":" + name);
//        //        debug(key, value2);
//        //        validate(fsa.tryToRecognize(key.c_str(), value2), "Failed to recognize " + key);
//    }
//    validate(ifs.eof(), "Failed to read the input file to the end");
//}

int main(int argc, char** argv) {
    validate(argc == 3, "Must provide exactly 2 arguments - input FSA filename and dictionary filename.");
    string fsaFilename = argv[1];
    string dictFilename = argv[2];
    Morfeusz morfeusz(fsaFilename);
    ifstream in;
    in.open(dictFilename.c_str());
    string line;
    while (getline(in, line)) {
        cerr << "TEST " << line << endl;
        vector<string> splitVector(split(line, '\t'));
        string orth = splitVector[0];
        string lemma = splitVector[1];
        string tag = splitVector[2];
        string name = splitVector[3];

        vector<MorphInterpretation> res;
        morfeusz.analyze(orth, res);
        bool found = false;

        for (unsigned int i = 0; i < res.size(); i++) {
            MorphInterpretation& mi = res[i];
            DEBUG("FOUND: " + mi.getLemma() + ":" + mi.getTag());
            if (lemma == mi.getLemma() && tag == mi.getTag() && name == mi.getName()) {
                DEBUG("RECOGNIZED " + orth + " " + lemma + ":" + tag + ":" + name);
                found = true;
            }
        }
        validate(found, "Failed to recognize " + orth + " " + lemma + ":" + tag + ":" + name);
    }
    return 0;
}
