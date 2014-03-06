/* 
 * File:   main.cc
 * Author: mlenart
 *
 * Created on October 8, 2013, 12:41 PM
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include "fsa/fsa.hpp"
#include "Tagset.hpp"
#include "Morfeusz.hpp"
#include "const.hpp"

using namespace std;

int main(int argc, char** argv) {
    Morfeusz morfeusz;
    if (argc > 1) {
        morfeusz.setAnalyzerFile(argv[1]);
        printf("Using dictionary from %s\n", argv[1]);
    }
#ifdef _WIN32
    morfeusz.setCharset(CP852);
#endif
#ifdef _WIN64
    morfeusz.setCharset(CP852);
#endif
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
            } else if (prevStart != -1) {
                printf("; ");
            }
            printf("%d,%d,%s,%s,%s,%s",
                    mi.getStartNode(), mi.getEndNode(),
                    mi.getOrth().c_str(), mi.getLemma().c_str(),
                    mi.getTag().c_str(), mi.getName().c_str());
            prevStart = mi.getStartNode();
            prevEnd = mi.getEndNode();
        }
        printf("]\n");
    }
    printf("\n");
    return 0;
}
