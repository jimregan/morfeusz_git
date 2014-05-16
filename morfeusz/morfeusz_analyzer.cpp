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
#include "Morfeusz.hpp"
#include "const.hpp"

#include "cli/cli.hpp"
#include "outputUtils.hpp"

using namespace std;

int main(int argc, const char** argv) {
    
    ez::ezOptionParser& opt = *getOptions(argc, argv, ANALYZER);
    Morfeusz morfeusz;
    initializeMorfeusz(opt, morfeusz);
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
