
#include <vector>
#include <string>
#include <cstdio>

#include "morfeusz2_version.h"
#include "morfeusz2_c.h"
#include "morfeusz2.h"
#include "c_api/ResultsManager.hpp"

using namespace std;
using namespace morfeusz;

static Morfeusz* morfeuszInstance = Morfeusz::createInstance();

extern "C" DLLIMPORT
char* morfeusz_about() {
    return const_cast<char*>(MORFEUSZ_VERSION);
}

extern "C" DLLIMPORT
InterpMorf* morfeusz_analyse(char *tekst) {
    static vector<MorphInterpretation> res;
    static ResultsManager resManager;
    res.clear();
    morfeuszInstance->analyze(tekst, res);
    return resManager.convertResults(res);
}

extern "C" DLLIMPORT
int morfeusz_set_option(int option, int value) {
    static int morfeusz_option_whitespace = 0;
    switch (option) {
        case MORFOPT_ENCODING:
            break;
//            return EncodedPtr::set_encoding(value);
        case MORFOPT_WHITESPACE:
            morfeusz_option_whitespace = value;
            return 1;
        default:
            fprintf(stderr, "Wrong option %d\n", option);
            return 0;
    }
    return 1;
}


