/* 
 * File:   morfeusz_generator.cpp
 * Author: mlenart
 *
 * Created on 21 styczeń 2014, 12:02
 */

#include <cstdlib>
#include <iostream>
#include <vector>
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
    cerr << "Morfeusz generator, version: " << MORFEUSZ_VERSION << endl;
    ez::ezOptionParser& opt = *getOptions(argc, argv, GENERATOR);
    MorfeuszInternal morfeusz;
    initializeMorfeusz(opt, morfeusz);
    string line;
    while (getline(cin, line)) {
        //        printf("%s\n", line.c_str());
        vector<MorphInterpretation> res;
        morfeusz.generate(line, res);
        printMorphResults(res, false);
    }
    printf("\n");
    delete &opt;
    return 0;
}
