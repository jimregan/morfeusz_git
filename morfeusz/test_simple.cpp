/* 
 * File:   test_simple.cpp
 * Author: lennyn
 *
 * Created on November 18, 2013, 10:30 PM
 */

#include <cstdlib>

#include "Morfeusz.hpp"
#include "MorphInterpretation.hpp"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    Morfeusz morfeusz(argv[1]);
    vector<MorphInterpretation> res;
    string word = "mijałem";
    const char* ptr = word.c_str();
    morfeusz.processOneWord(ptr, word.c_str() + word.size(), 0, res);
    for (MorphInterpretation& mi: res) {
        cerr << mi.getStartNode() << " " << mi.getEndNode() << " " << mi.getLemma() << " " << mi.getTag() << " " << mi.getName() << endl;
    }
    return 0;
}

