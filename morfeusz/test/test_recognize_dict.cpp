/* 
 * File:   test_morph.cpp
 * Author: mlenart
 *
 * Created on November 8, 2013, 4:12 PM
 */

//#include <cstdlib>
#include <sstream>
#include <iostream>
#include "utils.hpp"
#include "Morfeusz.hpp"
#include "MorphInterpretation.hpp"

using namespace std;
using namespace morfeusz;

int main(int argc, char** argv) {
    validate(argc == 3, "Must provide exactly 2 arguments - input FSA filename and dictionary filename.");
    string fsaFilename = argv[1];
    string dictFilename = argv[2];
    Morfeusz morfeusz;
    morfeusz.setAnalyzerDictionary(fsaFilename);
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
        cerr << "ANALYZE '" << orth << "'" << endl;
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
