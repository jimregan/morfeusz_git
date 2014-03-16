/* 
 * File:   Environment.hpp
 * Author: mlenart
 *
 * Created on 22 styczeń 2014, 12:08
 */

#ifndef ENVIRONMENT_HPP
#define	ENVIRONMENT_HPP

#include <vector>

class InterpretedChunksDecoder;

#include "charset/CaseConverter.hpp"
#include "charset/CharsetConverter.hpp"
#include "fsa/fsa.hpp"
#include "segrules/segrules.hpp"
#include "const.hpp"
#include "Tagset.hpp"
//#include "InterpretedChunksDecoder.hpp"
#include "InterpsGroup.hpp"

typedef FSA< std::vector<InterpsGroup > > FSAType;

class Environment {
public:
    Environment(
            MorfeuszCharset charset, 
            MorfeuszProcessorType morfeuszProcessor, 
            const unsigned char* fileStartPtr);
    
    void setCharset(MorfeuszCharset charset);
    
    const CharsetConverter& getCharsetConverter() const;
    
    const CaseConverter& getCaseConverter() const;
    
    void setTagset(const Tagset& tagset);
    const Tagset& getTagset() const;
    
    void setFSAFile(const std::string& filename);
    
    const SegrulesFSA& getCurrentSegrulesFSA() const;
    
    const FSAType& getFSA() const;
    
    const InterpretedChunksDecoder& getInterpretedChunksDecoder() const;
    
    virtual ~Environment();
private:
    const CharsetConverter* currentCharsetConverter;
    const UTF8CharsetConverter utf8CharsetConverter;
    const ISO8859_2_CharsetConverter isoCharsetConverter;
    const Windows_1250_CharsetConverter cp1250CharsetConverter;
    const CP852_CharsetConverter cp852CharsetConverter;
    const CaseConverter caseConverter;
    Tagset tagset;
    
    const unsigned char* fsaFileStartPtr;
    const FSAType* fsa;
    std::map<SegrulesOptions, SegrulesFSA*> segrulesFSAsMap;
    const SegrulesFSA* currSegrulesFSA;
    bool isFromFile;
    
    const InterpretedChunksDecoder* chunksDecoder;
    MorfeuszProcessorType processorType;
    
    const CharsetConverter* getCharsetConverter(MorfeuszCharset charset) const;
};

#endif	/* ENVIRONMENT_HPP */

