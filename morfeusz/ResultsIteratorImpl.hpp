/* 
 * File:   ResultsIteratorImpl.hpp
 * Author: mlenart
 *
 * Created on 26 czerwiec 2014, 14:37
 */

#ifndef RESULTSITERATORIMPL_HPP
#define	RESULTSITERATORIMPL_HPP

#include <string>
#include <vector>

#include "morfeusz2.h"
//#include "MorfeuszInternal.hpp"
#include "charset/TextReader.hpp"

namespace morfeusz {
    
    class MorfeuszInternal;
    
    class ResultsIteratorImpl: public ResultsIterator {
    public:
        
        ResultsIteratorImpl(const MorfeuszInternal& morfeusz, const char* text, const char* textEnd, bool isOwnerOfText);
        
        ~ResultsIteratorImpl();
        
        bool hasNext();
        
        const MorphInterpretation& peek();
        
        MorphInterpretation next();
        
    private:
        const MorfeuszInternal& morfeusz;
        const char* text;
        bool isOwnerOfText;
        TextReader reader;
        std::vector<MorphInterpretation> buffer;
        std::vector<MorphInterpretation>::iterator bufferIterator;
        
        bool tryToReadIntoBuffer();
        
        void ensureHasNext();
        
    };
}

#endif	/* RESULTSITERATORIMPL_HPP */

