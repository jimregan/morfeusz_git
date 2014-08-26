
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
        opt.example = string(argv[0]) + " --aggl strict --praet split --dict sgjp --dict-dir /tmp/dictionaries\n\n";
        //	opt.footer = "Morfeusz Copyright (C) 2014\n";

        opt.add(
                "", // Default.
                0, // Required?
                0, // Number of args expected.
                0, // Delimiter if expecting multiple args.
                "Display usage instructions.\n", // Help description.
                "-h", // Flag token. 
                //                "-help", // Flag token.
                "--help" // Flag token.
                );

        opt.add(
                "", // Default.
                0, // Required?
                1, // Number of args expected.
                0, // Delimiter if expecting multiple args.
                "dictionary name\n", // Help description.
                "-d", // Flag token. 
                //                "-dict", // Flag token.
                "--dict" // Flag token.
                );

        opt.add(
                "", // Default.
                0, // Required?
                1, // Number of args expected.
                0, // Delimiter if expecting multiple args.
                "directory containing the dictionary (optional)\n", // Help description.
                //                "-dict-dir", // Flag token. 
                "--dict-dir" // Flag token.
                );

        opt.add(
                "", // Default.
                0, // Required?
                1, // Number of args expected.
                0, // Delimiter if expecting multiple args.
                "select agglutination rules\n", // Help description.
                "-a", // Flag token. 
                //                "-aggl", // Flag token.
                "--aggl" // Flag token.
                );

        opt.add(
                "", // Default.
                0, // Required?
                1, // Number of args expected.
                0, // Delimiter if expecting multiple args.
                "select past tense segmentation\n", // Help description.
                "-p", // Flag token. 
                //                "-praet", // Flag token.
                "--praet" // Flag token.
                );

        opt.add(
                "", // Default.
                0, // Required?
                1, // Number of args expected.
                0, // Delimiter if expecting multiple args.
                "input/output charset (UTF8, ISO8859_2, CP1250, CP852)\n", // Help description.
                "-c", // Flag token. 
                //                "-charset", // Flag token.
                "--charset" // Flag token.
                );

        if (processorType == ANALYZER) {
            opt.add(
                    "", // Default.
                    0, // Required?
                    1, // Number of args expected.
                    0, // Delimiter if expecting multiple args.
                    "case handling strategy\n\
* CONDITIONALLY_CASE_SENSITIVE - Case-sensitive but allows interpretations that do not match case when there is no alternative\n\
* STRICTLY_CASE_SENSITIVE - strictly case-sensitive\n\
* IGNORE_CASE - ignores case\n", // Help description.
                    //                    "-case-handling", // Flag token.
                    "--case-handling" // Flag token.
                    );
            opt.add(
                    "", // Default.
                    0, // Required?
                    1, // Number of args expected.
                    0, // Delimiter if expecting multiple args.
                    "token numbering strategy\n\
* SEPARATE_NUMBERING - Start from 0 and reset counter for every line\n\
* CONTINUOUS_NUMBERING - start from 0 and never reset counter\n", // Help description.
                    //                    "-token-numbering", // Flag token.
                    "--token-numbering" // Flag token.
                    );
            opt.add(
                    "", // Default.
                    0, // Required?
                    1, // Number of args expected.
                    0, // Delimiter if expecting multiple args.
                    "whitespace handling strategy.\n \
* SKIP_WHITESPACES - ignore whitespaces\n \
* APPEND_WHITESPACES - append whitespaces to preceding segment\n\
* KEEP_WHITESPACES - whitespaces are separate segments\n", // Help description.
                    //                    "-whitespace-handling", // Flag token.
                    "--whitespace-handling" // Flag token.
                    );
        }

        opt.add(
                "", // Default.
                0, // Required?
                0, // Number of args expected.
                0, // Delimiter if expecting multiple args.
                "show some debug information.\n", // Help description.
                //                "-debug", // Flag token.
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
        if (optionStr == "SEPARATE_NUMBERING")
            return SEPARATE_NUMBERING;
        else if (optionStr == "CONTINUOUS_NUMBERING")
            return CONTINUOUS_NUMBERING;
        else {
            cerr << "Invalid token numbering: '" << optionStr << "'. Must be one of: SEPARATE_NUMBERING, CONTINUOUS_NUMBERING" << endl;
            throw "Invalid token numbering";
        }
    }

    static CaseHandling getCaseHandling(const string& optionStr) {
        if (optionStr == "CONDITIONALLY_CASE_SENSITIVE")
            return CONDITIONALLY_CASE_SENSITIVE;
        else if (optionStr == "STRICTLY_CASE_SENSITIVE")
            return STRICTLY_CASE_SENSITIVE;
        else if (optionStr == "IGNORE_CASE")
            return IGNORE_CASE;
        else {
            cerr << "Invalid case handling: '" << optionStr << "'. Must be one of: CONDITIONALLY_CASE_SENSITIVE, STRICTLY_CASE_SENSITIVE, IGNORE_CASE" << endl;
            throw "Invalid case handling";
        }
    }

    static WhitespaceHandling getWhitespaceHandling(const string& optionStr) {
        if (optionStr == "SKIP_WHITESPACES")
            return SKIP_WHITESPACES;
        else if (optionStr == "APPEND_WHITESPACES")
            return APPEND_WHITESPACES;
        else if (optionStr == "KEEP_WHITESPACES")
            return KEEP_WHITESPACES;
        else {
            cerr << "Invalid whitespace handling: '" << optionStr << "'. Must be one of: SKIP_WHITESPACES, APPEND_WHITESPACES, KEEP_WHITESPACES" << endl;
            throw "Invalid whitespace handling";
        }
    }

    Morfeusz* initializeMorfeusz(ezOptionParser& opt, MorfeuszProcessorType processorType) {
        if (opt.isSet("--dict-dir")) {
            string dictDir;
            opt.get("--dict-dir")->getString(dictDir);
            //            Morfeusz::dictionarySearchPaths.clear();
            Morfeusz::dictionarySearchPaths.push_front(dictDir);
            cerr << "Setting dictionary search path to: " << dictDir << endl;
        } else {
            Morfeusz::dictionarySearchPaths.push_front(".");
            cerr << "Setting dictionary search path to: ." << endl;
        }

        string dictName;
        if (opt.isSet("-d")) {
            opt.get("-d")->getString(dictName);
            cerr << "Using dictionary: " << dictName << endl;
        }
        else {
            dictName = Morfeusz::getDefaultDictName();
            cerr << "Using dictionary: " << dictName << " (default)" << endl;
        }

        Morfeusz& morfeusz = *Morfeusz::createInstance(dictName, processorType == ANALYZER ? ANALYSE_ONLY : GENERATE_ONLY);
        try {
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
            if (opt.isSet("--debug")) {
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
                if (opt.isSet("--case-handling")) {
                    string caseHandling;
                    opt.get("--case-handling")->getString(caseHandling);
                    cerr << "setting case handling to " << caseHandling << endl;
                    morfeusz.setCaseHandling(getCaseHandling(caseHandling));
                }

                if (opt.isSet("--token-numbering")) {
                    string tokenNumbering;
                    opt.get("--token-numbering")->getString(tokenNumbering);
                    cerr << "setting token numbering to " << tokenNumbering << endl;
                    morfeusz.setTokenNumbering(getTokenNumbering(tokenNumbering));
                }

                if (opt.isSet("--whitespace-handling")) {
                    string whitespaceHandling;
                    opt.get("--whitespace-handling")->getString(whitespaceHandling);
                    cerr << "setting whitespace handling to " << whitespaceHandling << endl;
                    morfeusz.setWhitespaceHandling(getWhitespaceHandling(whitespaceHandling));
                }
            }

#if defined(_WIN64) || defined(_WIN32)
            morfeusz.setCharset(CP852);
#endif
            return &morfeusz;
        } catch (const MorfeuszException& ex) {
            cerr << "Failed to start Morfeusz: " << ex.what() << endl;
            exit(1);
        }
    }

}
