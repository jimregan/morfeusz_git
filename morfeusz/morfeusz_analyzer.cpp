/* 
 * File:   main.cc
 * Author: mlenart
 *
 * Created on October 8, 2013, 12:41 PM
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>
#include "morfeusz2.h"
#include "cli/cli.hpp"

using namespace std;
using namespace morfeusz;

int main(int argc, const char** argv) {
    cerr << "Morfeusz analyzer, version: " << MORFEUSZ2_VERSION << endl;
    ez::ezOptionParser& opt = *getOptions(argc, argv, ANALYZER);
    Morfeusz* morfeusz = Morfeusz::createInstance();
    initializeMorfeusz(opt, *morfeusz, ANALYZER);
    
    string line;
    vector<MorphInterpretation> res;
    while (getline(cin, line)) {
//        printf("%s\n", line.c_str());
        res.clear();
        morfeusz->analyze(line, res);
        printMorphResults(res, true);
    }
    delete morfeusz;
    printf("\n");
    delete &opt;
    return 0;
}
