/* 
 * File:   test_morfeusz.cpp
 * Author: mlenart
 *
 * Created on 14 listopad 2013, 15:50
 */

#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <iostream>

#include "Morfeusz.hpp"

using namespace std;

void debug(const MorphInterpretation& interp) {
    fprintf(stderr, 
            "%d %d %s %s %s %s\n",
            interp.getStartNode(), interp.getEndNode(),
            interp.getOrth().c_str(), interp.getLemma().c_str(),
            interp.getTag().c_str(), interp.getName().c_str());
}

void doTest(
        const Morfeusz& morfeusz,
        const string& inputFilename) {
    ifstream ifs;
    //    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(inputFilename, ios::binary);
    string line;
    while (getline(ifs, line)) {
        DEBUG(line);
        vector<MorphInterpretation> res;
        morfeusz.analyze(line, res);
        for (MorphInterpretation& mi: res) {
            debug(mi);
        }
    }
    validate(ifs.eof(), "Failed to read the input file to the end");
}

int main(int argc, char** argv) {
    validate(argc == 3, "Must provide exactly two arguments - FSA filename, and input filename.");
    string fsaFilename = argv[1];
    string inputFilename = argv[2];
    DEBUG("FSA: "+fsaFilename);
    DEBUG("input text: "+inputFilename);
    Morfeusz morfeusz(fsaFilename);
    doTest(morfeusz, inputFilename);
    return 0;
}

