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
    cerr << "Morfeusz analyzer, version: " << Morfeusz::getVersion() << endl;
    ez::ezOptionParser& opt = *getOptions(argc, argv, ANALYZER);
    Morfeusz* morfeusz = initializeMorfeusz(opt, ANALYZER);
    string line;
    vector<MorphInterpretation> res;
    while (getline(cin, line)) {
        res.clear();
        morfeusz->analyse(line, res);
        printMorphResults(*morfeusz, res, true);
    }
    delete morfeusz;
    printf("\n");
    delete &opt;
    return 0;
}
