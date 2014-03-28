
#include <iostream>
#include <cstdlib>
#include "cli.hpp"

using namespace std;
using namespace ez;

static inline void printCLIUsage(ezOptionParser& opt, ostream& out) {
    string usage;
    opt.getUsage(usage);
    out << usage;
}

ezOptionParser* getOptions(int argc, const char** argv, const string& titleText) {

    ezOptionParser& opt = *(new ezOptionParser());

    opt.overview = titleText;
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
        exit(1);
    }

    if (!opt.lastArgs.empty()) {
        cerr << "Invalid argument (not bound to any flag): " << *opt.lastArgs[0] << endl;
        exit(1);
    }


    if (opt.isSet("-h")) {
        printCLIUsage(opt, cout);
        exit(0);
    }
    return &opt;
}

void initializeMorfeusz(ezOptionParser& opt, Morfeusz& morfeusz) {
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
}
//
//Morfeusz getMorfeuszFromCLI(int argc, const char** argv, const std::string& titleText) {
//    ezOptionParser opt = getOptions(argc, argv, titleText);
//    Morfeusz morfeusz;
//    initializeMorfeusz(opt, morfeusz);
//    return morfeusz;
//}
