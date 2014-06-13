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
#include <iostream>
#include "MorfeuszInternal.hpp"
#include "consoleUtils.hpp"

using namespace std;
using namespace morfeusz;

static Charset getEncoding(const string& encodingStr) {
    if (encodingStr == "UTF8")
        return UTF8;
    else if (encodingStr == "ISO8859_2")
        return ISO8859_2;
    else if (encodingStr == "CP1250")
        return CP1250;
    else if (encodingStr == "CP852")
        return CP852;
    else {
        cerr << "Invalid encoding: " << encodingStr << " must be one of: UTF8, ISO8859_2, WINDOWS1250" << endl;
        throw "Invalid encoding";
    }
}

int main(int argc, char** argv) {
    validate(argc == 3 || argc == 4, "Must provide exactly 2 or 3 arguments - input filename, required output filename, (optional) encoding.");
    string inputFilename = argv[1];
    ifstream in;
//    in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    cerr << "OPEN " << inputFilename << endl;
    in.open(inputFilename.c_str());
    string requiredOutputFilename = argv[2];
    ifstream requiredIn;
//    requiredIn.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    cerr << "OPEN " << requiredOutputFilename << endl;
    requiredIn.open(requiredOutputFilename.c_str());
    //    string requiredOutput = readFile<char>(requiredOutputFilename);
    cerr << "TEST START" << endl;
    MorfeuszInternal morfeusz;
    if (argc == 4) {
        Charset encoding = getEncoding(argv[3]);
        morfeusz.setCharset(encoding);
    }
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
            validate(gotOutputLine == requiredOutputLine, "lines do not match");
        }
    }
    return 0;
}

