
#include <string>
#include "morfeusz2.h"
#include "MorfeuszInternal.hpp"
#include "morfeusz2_version.h"

namespace morfeusz {

    using namespace std;

    string version() {
        static string version = MORFEUSZ_VERSION;
        return version;
    }

    Morfeusz* Morfeusz::createInstance() {
        return new MorfeuszInternal();
    }

    Morfeusz::~Morfeusz() {

    }

}
