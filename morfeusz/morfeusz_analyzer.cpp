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
#include "fsa/fsa.hpp"
#include "Tagset.hpp"
#include "MorfeuszInternal.hpp"
#include "morfeusz_version.h"
#include "const.hpp"

#include "cli/cli.hpp"
#include "cli/outputUtils.hpp"

using namespace std;
using namespace morfeusz;

int main(int argc, const char** argv) {
    cerr << "Morfeusz analyzer, version: " << MORFEUSZ_VERSION << endl;
    ez::ezOptionParser& opt = *getOptions(argc, argv, ANALYZER);
    MorfeuszInternal morfeusz;
    initializeMorfeusz(opt, morfeusz, ANALYZER);
//    Morfeusz morfeusz(getMorfeuszFromCLI(argc, argv, "Morfeusz analyzer"));
    
    string line;
    vector<MorphInterpretation> res;
    while (getline(cin, line)) {
//        printf("%s\n", line.c_str());
        res.clear();
        morfeusz.analyze(line, res);
        printMorphResults(res, true);
    }
    printf("\n");
    delete &opt;
    return 0;
}
