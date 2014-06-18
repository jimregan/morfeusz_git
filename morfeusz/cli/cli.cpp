
#include <iostream>
#include <cstdlib>
#include "cli.hpp"
#include "../const.hpp"

using namespace std;
using namespace ez;

namespace morfeusz {

    static inline void printCLIUsage(ezOptionParser& opt, ostream& out) {
        string usage;
        opt.getUsage(usage);
        out << usage;
    }

    ezOptionParser* getOptions(int argc, const char** argv, MorfeuszProcessorType processorType) {

        ezOptionParser& opt = *(new ezOptionParser());

        opt.overview = processorType == ANALYZER
                ? "Morfeusz analyzer"
                : "Morfeusz generator";
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

        opt.add(
                "", // Default.
                0, // Required?
                1, // Number of args expected.
                0, // Delimiter if expecting multiple args.
                "input/output charset", // Help description.
                "-c", // Flag token. 
                "-charset", // Flag token.
                "--charset" // Flag token.
                );

        if (processorType == ANALYZER) {
            opt.add(
                    "", // Default.
                    0, // Required?
                    1, // Number of args expected.
                    0, // Delimiter if expecting multiple args.
                    "case handling strategy.\n \
                     WEAK - Case-sensitive but allows interpretations that do not match case but there is no alternative\n \
                     STRICT - strictly case-sensitive\n \
                     IGNORE - ignores case\n", // Help description.
                    "-case-handling", // Flag token.
                    "--case-handling" // Flag token.
                    );
            opt.add(
                    "", // Default.
                    0, // Required?
                    1, // Number of args expected.
                    0, // Delimiter if expecting multiple args.
                    "token numbering strategy \
                     SEPARATE - Start from 0 and reset counter for every line\n \
                     CONTINUOUS - start from 0 and never reset counter", // Help description.
                    "-token-numbering", // Flag token.
                    "--token-numbering" // Flag token.
                    );
        }

        opt.add(
                "", // Default.
                0, // Required?
                0, // Number of args expected.
                0, // Delimiter if expecting multiple args.
                "show some debug information.", // Help description.
                "-d", // Flag token. 
                "-debug", // Flag token.
                "--debug" // Flag token.
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

    static Charset getCharset(const string& encodingStr) {
        if (encodingStr == "UTF8")
            return UTF8;
        else if (encodingStr == "ISO8859_2")
            return ISO8859_2;
        else if (encodingStr == "CP1250")
            return CP1250;
        else if (encodingStr == "CP852")
            return CP852;
        else {
            cerr << "Invalid encoding: '" << encodingStr << "'. Must be one of: UTF8, ISO8859_2, WINDOWS1250" << endl;
            throw "Invalid encoding";
        }
    }

    static TokenNumbering getTokenNumbering(const string& optionStr) {
        if (optionStr == "SEPARATE")
            return SEPARATE;
        else if (optionStr == "CONTINUOUS")
            return CONTINUOUS;
        else {
            cerr << "Invalid token numbering: '" << optionStr << "'. Must be one of: SEPARATE, CONTINUOUS" << endl;
            throw "Invalid token numbering";
        }
    }
    
    static CaseHandling getCaseHandling(const string& optionStr) {
        if (optionStr == "WEAK")
            return WEAK;
        else if (optionStr == "STRICT")
            return STRICT;
        else if (optionStr == "IGNORE")
            return IGNORE;
        else {
            cerr << "Invalid case handling: '" << optionStr << "'. Must be one of: WEAK, STRICT, IGNORE" << endl;
            throw "Invalid token numbering";
        }
    }

    void initializeMorfeusz(ezOptionParser& opt, Morfeusz& morfeusz, MorfeuszProcessorType processorType) {
        if (opt.isSet("-i")) {
            string dictFile;
            opt.get("-i")->getString(dictFile);
            switch (processorType) {
                case ANALYZER:
                    morfeusz.setAnalyzerDictionary(dictFile);
                    break;
                case GENERATOR:
                    morfeusz.setGeneratorDictionary(dictFile);
                    break;
                default:
                    break;
            }
            printf("Using dictionary from %s\n", dictFile.c_str());
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
        if (opt.isSet("-d")) {
            cerr << "setting debug to TRUE" << endl;
            morfeusz.setDebug(true);
        }
        if (opt.isSet("-c")) {
            string charset;
            opt.get("-c")->getString(charset);
            cerr << "setting charset to " << charset << endl;
            morfeusz.setCharset(getCharset(charset));
        }

        if (processorType == ANALYZER) {
            if (opt.isSet("-case-handling")) {
                string caseHandling;
                opt.get("-case-handling")->getString(caseHandling);
                cerr << "setting case handling to " << caseHandling << endl;
                morfeusz.setCaseHandling(getCaseHandling(caseHandling));
            }

            if (opt.isSet("-token-numbering")) {
                string tokenNumbering;
                opt.get("-token-numbering")->getString(tokenNumbering);
                cerr << "setting token numbering to " << tokenNumbering << endl;
                morfeusz.setTokenNumbering(getTokenNumbering(tokenNumbering));
            }

        }

#if defined(_WIN64) || defined(_WIN32)
        morfeusz.setCharset(CP852);
#endif
    }

}
