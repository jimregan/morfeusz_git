/* 
 * File:   ResultsIteratorImpl.hpp
 * Author: mlenart
 *
 * Created on 26 czerwiec 2014, 14:37
 */

#ifndef RESULTSITERATORIMPL_HPP
#define	RESULTSITERATORIMPL_HPP

#include <string>

#include "morfeusz2.h"
//#include "MorfeuszInternal.hpp"
#include "charset/TextReader.hpp"

namespace morfeusz {
    
    class MorfeuszInternal;
    
    class ResultsIteratorImpl: public ResultsIterator {
    public:
        
        ResultsIteratorImpl(const MorfeuszInternal& morfeusz, const std::string& text);
        
        virtual ~ResultsIteratorImpl();
        
        bool hasNext() const;
        
        const MorphInterpretation& peek() const;
        
        MorphInterpretation next();
        
    private:
        const MorfeuszInternal& morfeusz;
        TextReader reader;
    };
}

#endif	/* RESULTSITERATORIMPL_HPP */

