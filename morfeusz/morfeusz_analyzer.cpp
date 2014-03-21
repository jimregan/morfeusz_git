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
using namespace ez;

int main(int argc, const char** argv) {

    ezOptionParser opt;

    opt.overview = "Morfeusz analyzer";
    opt.syntax = string(argv[0]) + " [OPTIONS]";
    opt.example = string(argv[0]) + " --aggl strict --praet split --input /path/to/file.fsa";
    //	opt.footer = "Morfeusz Copyright (C) 2014\n";

    opt.add(
            "", // Default.
            0, // Required?
            0, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Display usage instructions.", // Help description.
            "-h", // Flag token. 
            "-help", // Flag token.
            "--help", // Flag token.
            "--usage" // Flag token.
            );

    opt.add(
            "", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "file with analyzer finite state automaton and data, created with buildfsa.py script.", // Help description.
            "-i", // Flag token. 
            "-input", // Flag token.
            "--input" // Flag token.
            );
    
    opt.add(
            "", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "aggl option.", // Help description.
            "-a", // Flag token. 
            "-aggl", // Flag token.
            "--aggl" // Flag token.
            );
    
    opt.add(
            "", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "praet option.", // Help description.
            "-p", // Flag token. 
            "-praet", // Flag token.
            "--praet" // Flag token.
            );

    opt.parse(argc, argv);
    
    if (opt.firstArgs.size() > 1) {
        cerr << "Invalid argument (not bound to any flag): " << *opt.firstArgs[1] << endl;
        return 1;
    }
    
    if (!opt.lastArgs.empty()) {
        cerr << "Invalid argument (not bound to any flag): " << *opt.lastArgs[0] << endl;
        return 1;
    }
    

    if (opt.isSet("-h")) {
        printCLIUsage(opt, cout);
        return 0;
    }

    Morfeusz morfeusz;
    if (opt.isSet("-i")) {
        string analyzerFile;
        opt.get("-i")->getString(analyzerFile);
        morfeusz.setAnalyzerFile(analyzerFile);
        printf("Using dictionary from %s\n", analyzerFile.c_str());
    }
    if (opt.isSet("-a")) {
        string aggl;
        opt.get("-a")->getString(aggl);
        cerr << "setting aggl option to " << aggl << endl;
        morfeusz.setAggl(aggl);
    }
    if (opt.isSet("-p")) {
        string praet;
        opt.get("-p")->getString(praet);
        cerr << "setting praet option to " << praet << endl;
        morfeusz.setPraet(praet);
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
            }
            else if (prevStart != -1) {
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
