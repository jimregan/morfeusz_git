/* 
 * File:   test_simple.cpp
 * Author: lennyn
 *
 * Created on November 18, 2013, 10:30 PM
 */

#include <cstdlib>

#include "utils.hpp"
#include "Morfeusz.hpp"
#include "MorphInterpretation.hpp"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    Morfeusz morfeusz("/tmp/test-SIMPLE-PoliMorfSmall.tab.fsa");
    DEBUG("żyję");
    vector<MorphInterpretation> res;
    string word = "  mijałem aa abadańską  fasASDfasd abdOminalności   ";
    morfeusz.analyze(word, res);
    DEBUG("znaleziono "+to_string(res.size()));
    for (MorphInterpretation& mi: res) {
        cerr << mi.getStartNode() << " " << mi.getEndNode() << " " << mi.getLemma() << " " << mi.getTag() << " " << mi.getName() << endl;
    }
    return 0;
}

