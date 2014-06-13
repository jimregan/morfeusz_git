/* 
 * File:   outputUtils.hpp
 * Author: mlenart
 *
 * Created on 15 maj 2014, 17:45
 */

#ifndef OUTPUTUTILS_HPP
#define	OUTPUTUTILS_HPP

#include "MorphInterpretation.hpp"

namespace morfeusz {

void printMorphResults(const std::vector<MorphInterpretation>& res, bool printNodeNumbers) {
    printf("[");
    int prevStart = -1;
    int prevEnd = -1;
    for (unsigned int i = 0; i < res.size(); i++) {
        const MorphInterpretation& mi = res[i];
        if (prevStart != -1
                && (prevStart != mi.getStartNode() || prevEnd != mi.getEndNode())) {
            printf("]\n[");
        }
        else if (prevStart != -1) {
            printf("\n ");
        }
        //            printf("%s", mi.toString(true).c_str());
        if (printNodeNumbers) {
            printf("%d,%d,", mi.getStartNode(), mi.getEndNode());
        }
        printf("%s,%s,%s,%s,",
                mi.getOrth().c_str(),
                mi.getLemma().c_str(),
                mi.getTag().c_str(),
                mi.getName().empty() ? "_" : mi.getName().c_str());
        if (!mi.getQualifiers().empty()) {
            printf("%s", mi.getQualifiers()[0].c_str());
            for (unsigned int i = 1; i < mi.getQualifiers().size(); i++) {
                printf("|%s", mi.getQualifiers()[i].c_str());
            }
        }
        else {
            printf("_");
        }
        prevStart = mi.getStartNode();
        prevEnd = mi.getEndNode();
    }
    printf("]\n");
}

}

#endif	/* OUTPUTUTILS_HPP */

