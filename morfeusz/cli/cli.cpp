
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
                "dictionary name", // Help description.
                "-d", // Flag token. 
                "-dict", // Flag token.
                "--dict" // Flag token.
                );
        
        opt.add(
                "", // Default.
                0, // Required?
                1, // Number of args expected.
                0, // Delimiter if expecting multiple args.
                "directory containing the dictionary (optional)", // Help description.
                "-dict-dir", // Flag token. 
                "--dict-dir" // Flag token.
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
            opt.add(
                    "", // Default.
                    0, // Required?
                    1, // Number of args expected.
                    0, // Delimiter if expecting multiple args.
                    "whitespace handling strategy. \n \
                     SKIP - ignore whitespaces \n \
                     APPEND - append whitespaces to preceding segment\n \
                     KEEP - whitespaces are separate segments", // Help description.
                    "-whitespace-handling", // Flag token.
                    "--whitespace-handling" // Flag token.
                    );
        }

        opt.add(
                "", // Default.
                0, // Required?
                0, // Number of args expected.
                0, // Delimiter if expecting multiple args.
                "show some debug information.", // Help description.
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
            return SEPARATE_NUMBERING;
        else if (optionStr == "CONTINUOUS")
            return CONTINUOUS_NUMBERING;
        else {
            cerr << "Invalid token numbering: '" << optionStr << "'. Must be one of: SEPARATE, CONTINUOUS" << endl;
            throw "Invalid token numbering";
        }
    }
    
    static CaseHandling getCaseHandling(const string& optionStr) {
        if (optionStr == "WEAK")
            return CONDITIONALLY_CASE_SENSITIVE;
        else if (optionStr == "STRICT")
            return STRICTLY_CASE_SENSITIVE;
        else if (optionStr == "IGNORE")
            return IGNORE_CASE;
        else {
            cerr << "Invalid case handling: '" << optionStr << "'. Must be one of: WEAK, STRICT, IGNORE" << endl;
            throw "Invalid token numbering";
        }
    }
    
    static WhitespaceHandling getWhitespaceHandling(const string& optionStr) {
        if (optionStr == "SKIP")
            return SKIP_WHITESPACES;
        else if (optionStr == "APPEND")
            return APPEND_WHITESPACES;
        else if (optionStr == "KEEP")
            return KEEP_WHITESPACES;
        else {
            cerr << "Invalid whitespace handling: '" << optionStr << "'. Must be one of: SKIP, APPEND, KEEP" << endl;
            throw "Invalid whitespace handling";
        }
    }

    Morfeusz* initializeMorfeusz(ezOptionParser& opt, MorfeuszProcessorType processorType) {
        Morfeusz& morfeusz = *Morfeusz::createInstance();
        if (opt.isSet("-d")) {
            string dictName;
            opt.get("-d")->getString(dictName);
            morfeusz.setDictionary(dictName);
            cerr << "Using dictionary: " << dictName << endl;
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
        if (opt.isSet("-debug")) {
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
            
            if (opt.isSet("-whitespace-handling")) {
                string whitespaceHandling;
                opt.get("-whitespace-handling")->getString(whitespaceHandling);
                cerr << "setting whitespace handling to " << whitespaceHandling << endl;
                morfeusz.setWhitespaceHandling(getWhitespaceHandling(whitespaceHandling));
            }
        }
        
#if defined(_WIN64) || defined(_WIN32)
        morfeusz.setCharset(CP852);
#endif
        return &morfeusz;
    }

}
