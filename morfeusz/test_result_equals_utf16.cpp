/* 
 * File:   test_result_equals_utf16.cpp
 * Author: lennyn
 *
 * Created on December 12, 2013, 12:16 PM
 */

#include <cstdlib>
#include <cassert>

using namespace std;

static MorfeuszCharset getEncoding(const string& encodingStr) {
    if (encodingStr == "UTF16_BE")
        return UTF16BE;
    else if (encodingStr == "UTF16_LE")
        return UTF16LE;
    else {
        cerr << "Invalid encoding: " << encodingStr << " must be one of: UTF16_BE, UTF16_LE" << endl;
        assert(false);
    }
}

string readFile(const char* fname) {
    std::ifstream ifs;
    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(fname, std::ios::in | std::ios::binary | std::ios::ate);
    //    if (ifs.is_open()) {
    long size = ifs.tellg();
    T* memblock = new T[size];
    ifs.seekg(0, std::ios::beg);
    ifs.read(reinterpret_cast<char*> (memblock), size);
    ifs.close();
    return memblock;
    //    } 
    //    else {
    //        cerr << "Unable to open file " << fname << endl;
    //    }
}

/*
 * 
 */
int main(int argc, char** argv) {
    validate(argc == 4, "Must provide exactly 2 or 3 arguments - input filename, required output filename, encoding.");
    string inputFilename = argv[1];
    ifstream in;
    //    in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    cerr << "OPEN " << inputFilename << endl;
    in.open(inputFilename);
    string requiredOutputFilename = argv[2];
    ifstream requiredIn;
    //    requiredIn.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    cerr << "OPEN " << requiredOutputFilename << endl;
    requiredIn.open(requiredOutputFilename);
    //    string requiredOutput = readFile<char>(requiredOutputFilename);
    cerr << "TEST START" << endl;
    Morfeusz morfeusz;
    MorfeuszCharset encoding = getEncoding(argv[3]);
    morfeusz.setEncoding(encoding);
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

