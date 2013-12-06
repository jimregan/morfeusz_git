/* 
 * File:   test_result_equals.cpp
 * Author: lennyn
 *
 * Created on December 6, 2013, 12:45 PM
 */

#include <cstdlib>
#include <cassert>
#include <string>
#include <sstream>
#include <fstream>
#include "Morfeusz.hpp"
#include "utils.hpp"

using namespace std;

//string doRunTest(
//        const Morfeusz& morfeusz,
//        const string& inputFilename) {
//    stringstream res;
//    ifstream ifs;
//    //    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//    ifs.open(inputFilename, ios::binary);
//    string line;
//    while (getline(ifs, line)) {
//        DEBUG(line);
//        vector<MorphInterpretation> res;
//        morfeusz.analyze(line, res);
//        for (MorphInterpretation& mi: res) {
//            debug(mi);
//        }
//    }
//    validate(ifs.eof(), "Failed to read the input file to the end");
//}

int main(int argc, char** argv) {
    validate(argc == 3, "Must provide exactly 2 arguments - input filename, required output filename.");
    string inputFilename = argv[1];
    ifstream in(inputFilename);
    string requiredOutputFilename = argv[2];
    ifstream requiredIn(requiredOutputFilename);
    //    string requiredOutput = readFile<char>(requiredOutputFilename);
    cerr << "TEST START" << endl;
    Morfeusz morfeusz;
    string line;
    while (getline(in, line)) {
        cerr << "TEST " << line << endl;
        vector<MorphInterpretation> res;
        morfeusz.analyze(line, res);
        stringstream out;
        appendMorfeuszResults(res, out);
        string gotOutputLine;
        string requiredOutputLine;
        while (getline(out, gotOutputLine)) {
            getline(requiredIn, requiredOutputLine);
            cerr << "REQUIRED LINE " << requiredOutputLine << endl;
            cerr << "GOT LINE " << gotOutputLine << endl;
            cerr << (requiredOutputLine == gotOutputLine) << endl;
            assert(gotOutputLine == requiredOutputLine);
        }
    }
    return 0;
}

