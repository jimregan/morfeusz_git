
#include "morfeusz2.h"
#include "MorfeuszInternal.hpp"

namespace morfeusz {

Morfeusz* Morfeusz::createInstance() {
    return new MorfeuszInternal();
}

Morfeusz::~Morfeusz() {
    
}
        
}
