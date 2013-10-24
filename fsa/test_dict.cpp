/* 
 * File:   test.cpp
 * Author: lennyn
 *
 * Created on October 22, 2013, 2:11 PM
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <vector>
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

void testFSA(const FSA<char*>& fsa, const char* fname) {
    ifstream ifs;
//    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(fname, ios::binary);
    string line;
    while (getline(ifs, line)) {
        vector<string> split1(split(line, '\t'));
        string key = split1[0];
        key = "bijekcją";
        string value = split1[1];
        
        for (unsigned int i = 0; i < key.length(); i++) {
            cout << (int) key[i] << " ";
        }
        cout << endl;
        
        char* value2;
        if (fsa.tryToRecognize(key.c_str(), value2)) {
            if (string(value) != string(value2)) {
                cout << "BAD INTERP " << key << " " << value << " != " << value2 << endl;
            }
            else {
                cout << "OK! " << key << " " << value << endl;
            }
        }
        else {
            cout << "MISS " << key << " " << value << " not recognized" << endl;
        }
    }
    cout << ifs.good() << endl;
    cout << ifs.fail() << endl;
    cout << ifs.eof() << endl;
    cout << "done" << endl;
}

/*
 * 
 */
int main(int argc, char** argv) {
    validate(argc == 3, "Must provide exactly two arguments - FSA filename and dictionary filename.");
    const unsigned char* fsaData = readFile(argv[1]);
    StringDeserializer deserializer;
    SimpleFSA<char*> fsa(fsaData, deserializer);
    testFSA(fsa, argv[2]);
    cout << argc << endl;
    return 0;
}

