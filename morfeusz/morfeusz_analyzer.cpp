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

using namespace std;

int main(int argc, const char** argv) {
    
    ez::ezOptionParser& opt = *getOptions(argc, argv, ANALYZER);
    Morfeusz morfeusz;
    initializeMorfeusz(opt, morfeusz);
//    Morfeusz morfeusz(getMorfeuszFromCLI(argc, argv, "Morfeusz analyzer"));
    
    string line;
    while (getline(cin, line)) {
        //        printf("%s\n", line.c_str());
        vector<MorphInterpretation> res;
        morfeusz.analyze(line, res);
        int prevStart = -1;
        int prevEnd = -1;
        printf("[");
        for (unsigned int i = 0; i < res.size(); i++) {
            MorphInterpretation& mi = res[i];
            if (prevStart != -1
                    && (prevStart != mi.getStartNode() || prevEnd != mi.getEndNode())) {
                printf("]\n[");
            }
            else if (prevStart != -1) {
                printf("; ");
            }
            printf("%s", mi.toString(true).c_str());
//            printf("%d,%d,%s,%s,%s,%s",
//                    mi.getStartNode(), mi.getEndNode(),
//                    mi.getOrth().c_str(), lemmaToShow.c_str(),
//                    mi.getTag().c_str(), lemmaToShow.c_str());
            prevStart = mi.getStartNode();
            prevEnd = mi.getEndNode();
        }
        printf("]\n");
    }
    printf("\n");
    delete &opt;
    return 0;
}
