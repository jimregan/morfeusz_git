/* 
 * File:   morfeusz_generator.cpp
 * Author: mlenart
 *
 * Created on 21 styczeń 2014, 12:02
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include "morfeusz2.h"
#include "cli/cli.hpp"

using namespace std;
using namespace morfeusz;

int main(int argc, const char** argv) {
    cerr << "Morfeusz generator, version: " << Morfeusz::getVersion() << endl;
    ez::ezOptionParser& opt = *getOptions(argc, argv, GENERATOR);
    Morfeusz* morfeusz = initializeMorfeusz(opt, GENERATOR);
    string line;
    vector<MorphInterpretation> res;
    try {
        while (getline(cin, line)) {
         //        printf("%s\n", line.c_str());
            res.clear();
            morfeusz->generate(line, res);
            printMorphResults(*morfeusz, res, false);
        }
    }
    catch (const std::exception& ex) {
        cerr << "Failed to perform morphosyntactic synthesis: " << ex.what() << endl;
        exit(1);
    }
    printf("\n");
    delete &opt;
    return 0;
}
