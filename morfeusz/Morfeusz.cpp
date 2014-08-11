
#include <string>
#include "morfeusz2.h"
#include "MorfeuszImpl.hpp"

namespace morfeusz {

    using namespace std;

    string Morfeusz::getVersion() {
        static string version = MORFEUSZ2_VERSION;
        return version;
    }

    Morfeusz* Morfeusz::createInstance(MorfeuszUsage usage) {
        return new MorfeuszImpl(usage);
    }

    Morfeusz::~Morfeusz() {

    }
    
    list<string> Morfeusz::dictionarySearchPaths;

}
