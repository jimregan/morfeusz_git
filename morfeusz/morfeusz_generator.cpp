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
#include "Morfeusz.hpp"
#include "const.hpp"
#include "cli/cli.hpp"

using namespace std;

int main(int argc, const char** argv) {
    ez::ezOptionParser opt = getOptions(argc, argv, "Morfeusz analyzer");
    Morfeusz morfeusz;
    initializeMorfeusz(opt, morfeusz);
    string line;
    while (getline(cin, line)) {
        //        printf("%s\n", line.c_str());
        vector<MorphInterpretation> res;
        morfeusz.generate(line, res);
        printf("[");
        for (unsigned int i = 0; i < res.size(); i++) {
            if (i > 0) {
                printf("; ");
            }
            MorphInterpretation& mi = res[i];
            printf("%s", mi.toString(false).c_str());
        }
        printf("]\n");
    }
    printf("\n");
    return 0;
}
