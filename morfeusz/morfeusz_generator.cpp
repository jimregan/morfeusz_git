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

using namespace std;

int main(int argc, char** argv) {
    Morfeusz morfeusz;
    if (argc > 1) {
        morfeusz.setGeneratorFile(argv[1]);
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
        morfeusz.generate(line, res);
        printf("[");
        for (unsigned int i = 0; i < res.size(); i++) {
            if (i > 0) {
                printf("; ");
            }
            MorphInterpretation& mi = res[i];
            printf("%s,%s,%s,%s",
                    mi.getOrth().c_str(), mi.getLemma().c_str(),
                    mi.getTag().c_str(), mi.getName().c_str());
        }
        printf("]\n");
    }
    printf("\n");
    return 0;
}
