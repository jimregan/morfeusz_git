
#include <string>
#include "morfeusz2.h"
#include "MorfeuszInternal.hpp"

namespace morfeusz {

    using namespace std;

    string Morfeusz::getVersion() {
        static string version = MORFEUSZ2_VERSION;
        return version;
    }

    Morfeusz* Morfeusz::createInstance() {
        return new MorfeuszInternal();
    }

    Morfeusz::~Morfeusz() {

    }

}
