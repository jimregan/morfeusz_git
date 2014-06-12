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

#include "case/CaseConverter.hpp"
#include "charset/CharsetConverter.hpp"
#include "fsa/fsa.hpp"
#include "segrules/segrules.hpp"
#include "const.hpp"
#include "Tagset.hpp"
#include "InterpsGroup.hpp"
#include "case/CasePatternHelper.hpp"
#include "Qualifiers.hpp"
#include "deserialization/InterpsGroupsReader.hpp"

struct InterpsGroup;
typedef FSA<InterpsGroupsReader> FSAType;

/**
 * This class contains data required for morphological analysis/synthesis.
 * It contains references to dictionary automaton, charset converter, tagset data etc.
 * All of these can be changed by setters, changing Morfeusz behavior (different dictionary, charset, and other options).
 */
class Environment {
public:
    /**
     * Creates default environment with given initial charset, processor type (analyzer/generator) and default dictionary data ptr.
     * 
     * @param charset
     * @param morfeuszProcessor
     * @param fileStartPtr
     */
    Environment(
            MorfeuszCharset charset,
            MorfeuszProcessorType morfeuszProcessor,
            const unsigned char* fileStartPtr);
    
    /**
     * Sets charset for this environment.
     * 
     * @param charset
     */
    void setCharset(MorfeuszCharset charset);
    
    /**
     * Sets case sensitivity options.
     * 
     * @param caseSensitive - if true, interpretations not matching case will be discarded.
     */
    void setCaseSensitive(bool caseSensitive);
    
    /**
     * Gets charset converter that is currently used by this environment.
     * Changed by setting charset.
     * 
     * @return - reference to charset converter.
     */
    const CharsetConverter& getCharsetConverter() const;
    
    /**
     * Returns case converter that is currently used by this environment.
     * Changed by setting case sensitivity option.
     * 
     * @return - reference to case converter.
     */
    const CaseConverter& getCaseConverter() const;
    
    /**
     * Sets new tagset for this environment.
     * 
     * @param tagset
     */
    void setTagset(const Tagset& tagset);
    
    /**
     * Gets currently used tagset.
     * 
     * @return 
     */
    const Tagset& getTagset() const;
    
    /**
     * Sets binary dictionary file used by this environment.
     * 
     * @param filename - filename of the dictionary
     */
    void setFSAFile(const std::string& filename);
    
    /**
     * Sets segmentation rules option.
     * 
     * @param option
     * @param value
     */
    void setSegrulesOption(const std::string& option, const std::string& value);
    
    /**
     * Gets segmentation rules automaton.
     * 
     * @return 
     */
    const SegrulesFSA& getCurrentSegrulesFSA() const;
    
    /**
     * Gets dictionary automaton.
     * 
     * @return 
     */
    const FSAType& getFSA() const;
    
    /**
     * Returns decoder that converts interpretations to external format.
     * @return 
     */
    const InterpretedChunksDecoder& getInterpretedChunksDecoder() const;
    
    /**
     * Gets processor type (info if this is analyzer or generator environment)
     * @return 
     */
    MorfeuszProcessorType getProcessorType() const;
    
    /**
     * Return current case pattern helper
     * 
     * @return 
     */
    const CasePatternHelper& getCasePatternHelper() const;
    
    /**
     * Return current qualifiers helper.
     * @return 
     */
    const Qualifiers& getQualifiersHelper() const;
    
    /**
     * Returns true iff given codepoint denotes a separator char for ign handling.
     * @param codepoint
     * @return 
     */
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

