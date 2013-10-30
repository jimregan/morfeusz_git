/* 
 * File:   test_recognize.cpp
 * Author: lennyn
 *
 * Created on October 30, 2013, 5:26 PM
 */

#include <cstdlib>
#include <sstream>
#include "fsa.hpp"
#include "utils.hpp"

using namespace std;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

static inline string &rtrim(string &s) {
        s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
        return s;
}

void doTest(const FSA<char*>& fsa, const char* fname) {
    ifstream ifs;
//    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(fname, ios::binary);
    string line;
    while (getline(ifs, line)) {
        
        vector<string> splitVector(split(line, '\t'));
        string key = splitVector[0];
        
        cerr << "test " << key << endl;
        
        char* value2;
        validate(fsa.tryToRecognize(key.c_str(), value2), "Failed to recognize "+key);
    }
//    validate(ifs.good(), "Something wrong with the input file");
//    validate(!ifs.fail(), "Something wrong with the input file");
    validate(ifs.eof(), "Failed to read the input file to the end");
}

int main(int argc, char** argv) {
    cerr << (int) ((unsigned char) -123) << endl;
    validate(argc == 3, "Must provide exactly two arguments - FSA filename and dictionary filename.");
    const unsigned char* fsaData = readFile(argv[1]);
    StringDeserializer deserializer;
    FSAImpl<char*> fsa(fsaData, deserializer);
    doTest(fsa, argv[2]);
//    cout << argc << endl;
    return 0;
}

