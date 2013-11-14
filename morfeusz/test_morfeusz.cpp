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
            interp.getOrth(), interp.getLemma(),
            interp.getTag(), interp.getName());
}

void doTest(
        const Morfeusz& morfeusz,
        const string& inputFilename) {
    ifstream ifs;
    //    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(inputFilename, ios::binary);
    string line;
    while (getline(ifs, line)) {
        AnalyzeResult res(morfeusz.analyze(line));
        while (res.iterator != res.end) {
            debug(*res);
            res++;
        }
    }
    validate(ifs.eof(), "Failed to read the input file to the end");
}

int main(int argc, char** argv) {
    validate(argc == 3, "Must provide exactly two arguments - FSA filename, and input filename.");
    string fsaFilename = argv[1];
    string inputFilename = argv[2];
    Morfeusz morfeusz(fsaFilename);
    doTest(morfeusz, inputFilename);
    return 0;
}

