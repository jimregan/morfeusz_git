
#include <string>
#include "morfeusz2.h"
#include "MorfeuszImpl.hpp"

namespace morfeusz {

    using namespace std;

    string Morfeusz::getVersion() {
        static string version = MORFEUSZ2_VERSION;
        return version;
    }

    string Morfeusz::getDefaultDictName() {
        static string dictName = MORFEUSZ_DEFAULT_DICT_NAME;
        return dictName;
    }

    Morfeusz* Morfeusz::createInstance(MorfeuszUsage usage) {
        return new MorfeuszImpl(usage);
    }

    Morfeusz::~Morfeusz() {

    }

    static list<string> initializeDictionarySearchPaths() {
        list<string> res;
#ifdef MORFEUSZ_DICTIONARY_SEARCH_PATH1
        res.push_back(MORFEUSZ_DICTIONARY_SEARCH_PATH1);
#endif
#ifdef MORFEUSZ_DICTIONARY_SEARCH_PATH2
        res.push_back(MORFEUSZ_DICTIONARY_SEARCH_PATH2);
#endif
#ifdef MORFEUSZ_DICTIONARY_SEARCH_PATH3
        res.push_back(MORFEUSZ_DICTIONARY_SEARCH_PATH3);
#endif
        return res;
    }

    list<string> Morfeusz::dictionarySearchPaths(initializeDictionarySearchPaths());

}
