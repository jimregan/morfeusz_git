/* 
 * File:   ResultsIteratorImpl.cpp
 * Author: mlenart
 * 
 * Created on 26 czerwiec 2014, 14:37
 */

#include "ResultsIteratorImpl.hpp"
#include "MorfeuszInternal.hpp"

namespace morfeusz {

    ResultsIteratorImpl::ResultsIteratorImpl(const MorfeuszInternal& morfeusz, const std::string& text)
    : morfeusz(morfeusz), reader(text, morfeusz.analyzerEnv) {
    }
    
    ResultsIteratorImpl::~ResultsIteratorImpl() {
    }

    bool ResultsIteratorImpl::hasNext() const {
        
    }

    const MorphInterpretation& ResultsIteratorImpl::peek() const {
        
    }

    MorphInterpretation ResultsIteratorImpl::next() {
        
    }
}

