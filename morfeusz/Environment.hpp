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
class CasePatternHelper;

#include "charset/CaseConverter.hpp"
#include "charset/CharsetConverter.hpp"
#include "fsa/fsa.hpp"
#include "segrules/segrules.hpp"
#include "const.hpp"
#include "Tagset.hpp"
//#include "InterpretedChunksDecoder.hpp"
#include "InterpsGroup.hpp"
#include "CasePatternHelper.hpp"
#include "Qualifiers.hpp"

struct InterpsGroup;
typedef FSA< std::vector<InterpsGroup > > FSAType;

class Environment {
public:
    Environment(
            MorfeuszCharset charset,
            MorfeuszProcessorType morfeuszProcessor,
            const unsigned char* fileStartPtr);
    
    void setCharset(MorfeuszCharset charset);
    
    void setCaseSensitive(bool caseSensitive);
    
    const CharsetConverter& getCharsetConverter() const;
    
    const CaseConverter& getCaseConverter() const;
    
    void setTagset(const Tagset& tagset);
    const Tagset& getTagset() const;
    
    void setFSAFile(const std::string& filename);
    
    void setSegrulesOption(const std::string& option, const std::string& value);
    
    const SegrulesFSA& getCurrentSegrulesFSA() const;
    
    const FSAType& getFSA() const;
    
    const InterpretedChunksDecoder& getInterpretedChunksDecoder() const;
    
    MorfeuszProcessorType getProcessorType() const;
    
    const CasePatternHelper& getCasePatternHelper() const;
    
    const Qualifiers& getQualifiersHelper() const;
    
    bool isSeparator(uint32_t codepoint) const;
    
    virtual ~Environment();
private:
    const CharsetConverter* currentCharsetConverter;
    const CaseConverter caseConverter;
    Tagset tagset;
    Qualifiers qualifiers;
    
    const unsigned char* fsaFileStartPtr;
    const FSAType* fsa;
    std::vector<uint32_t> separatorsList;
    std::map<SegrulesOptions, SegrulesFSA*> segrulesFSAsMap;
    SegrulesOptions currSegrulesOptions;
    const SegrulesFSA* currSegrulesFSA;
    bool isFromFile;
    
    const InterpretedChunksDecoder* chunksDecoder;
    MorfeuszProcessorType processorType;
    CasePatternHelper* casePatternHelper;
    
    const CharsetConverter* getCharsetConverter(MorfeuszCharset charset) const;
};

#endif	/* ENVIRONMENT_HPP */

