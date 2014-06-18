/* 
 * File:   Morfeusz.hpp
 * Author: mlenart
 *
 * Created on November 13, 2013, 5:21 PM
 */

#ifndef MORFEUSZ_HPP
#define	MORFEUSZ_HPP

#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>

#include "morfeusz2.h"

#include "fsa/fsa.hpp"
#include "InterpsGroup.hpp"
#include "case/CaseConverter.hpp"
#include "charset/CharsetConverter.hpp"
#include "charset/TextReader.hpp"
#include "InterpretedChunk.hpp"
#include "InflexionGraph.hpp"
#include "MorfeuszOptions.hpp"
#include "const.hpp"
#include "exceptions.hpp"
#include "Environment.hpp"

#include "segrules/segrules.hpp"
#include "segrules/SegrulesFSA.hpp"

#include "deserialization/InterpsGroupsReader.hpp"
#include "deserialization/MorphDeserializer.hpp"

namespace morfeusz {

class MorfeuszInternal;
class MorphInterpretation;
class ResultsIterator;

typedef State<InterpsGroupsReader> StateType;

/**
 * Performs morphological analysis (analyze methods) and syntesis (generate methods).
 * 
 * It is NOT thread-safe
 * but it is possible to use separate Morfeusz instance for each concurrent thread.
 */
class MorfeuszInternal: public Morfeusz {
public:
    
    /**
     * Create new instance of Morfeusz class.
     */
    MorfeuszInternal();
    
    /**
     * Set a file used for morphological analysis.
     * 
     * @param filename
     */
    void setAnalyzerDictionary(const std::string& filename);
    
    /**
     * Set a file used for morphological synthesis.
     * 
     * @param filename
     */
    void setGeneratorDictionary(const std::string& filename);
    
    /**
     * Destroys Morfeusz object.
     */
    virtual ~MorfeuszInternal();
    
    /**
     * Analyze given text and return the results as iterator.
     * 
     * @param text - text for morphological analysis
     * @return - iterator over morphological analysis results
     */
    ResultsIterator analyze(const std::string& text) const;
    
    /**
     * Perform morphological analysis on a given text and put results in a vector.
     * 
     * @param text - text to be analyzed
     * @param result - results vector
     */
    void analyze(const std::string& text, std::vector<MorphInterpretation>& result) const;
    
    /**
     * Perform morphological synthesis on a given lemma and return the results as iterator.
     * 
     * @param text - text for morphological analysis
     * @return - iterator over morphological analysis results
     */
    ResultsIterator generate(const std::string& lemma) const;
    
    /**
     * Perform morphological synthesis on a given lemma and return the results as iterator.
     * Limit results to interpretations with the specified tag.
     * 
     * @param text - text for morphological analysis
     * @param tag - tag of result interpretations
     * @return - iterator over morphological analysis results
     */
    ResultsIterator generate(const std::string& lemma, int tagnum) const;

    /**
     * Perform morphological synthesis on a given lemma and put results in a vector.
     * 
     * @param lemma - lemma to be analyzed
     * @param result - results vector
     */
    void generate(const std::string& lemma, std::vector<MorphInterpretation>& result) const;
    
    /**
     * Perform morphological synthesis on a given lemma and put results in a vector.
     * Limit results to interpretations with the specified tag.
     * 
     * @param lemma - lemma to be analyzed
     * @param tag - tag of result interpretations
     * @param result - results vector
     */
    void generate(const std::string& lemma, int tagnum, std::vector<MorphInterpretation>& result) const;

    /**
     * Set encoding for input and output string objects.
     * 
     * @param encoding
     */
    void setCharset(Charset encoding);
    
    /**
     * Set aggl segmentation option value.
     * 
     * @param aggl
     */
    void setAggl(const std::string& aggl);
    
    /**
     * Set praet segmentation option value.
     * 
     * @param praet
     */
    void setPraet(const std::string& praet);
    
    /**
     * If set to true characters case in analyzed text must match
     * the case in the recognized forms from dictionary.
     * 
     * @param caseSensitive
     */
    void setCaseSensitive(bool caseSensitive);
    
    void setTokenNumbering(TokenNumbering tokenNumbering);
    
    /**
     * Set debug option value.
     * 
     * @param debug
     */
    void setDebug(bool debug);

    friend class ResultsIterator;
private:
    
    void processOneWord(
            const Environment& env,
            TextReader& reader,
            int startNodeNum,
            std::vector<MorphInterpretation>& result,
            bool insideIgnHandler=false) const;

    void doProcessOneWord(
            const Environment& env,
            TextReader& reader,
            const SegrulesState& segrulesState) const;
    
    void handleIgnChunk(
        const Environment& env,
        const char* inputStart,
        const char* inputEnd,
        int startNodeNum,
        std::vector<MorphInterpretation>& results) const;

    void appendIgnotiumToResults(
            const Environment& env,
            const std::string& word,
            int startNodeNum,
            std::vector<MorphInterpretation>& results) const;
    
    Environment analyzerEnv;
    Environment generatorEnv;
    MorfeuszOptions options;
    mutable std::vector<InterpretedChunk> accum;
    mutable InflexionGraph graph;
    mutable int nextNodeNum;
};

}

#endif	/* MORFEUSZ_HPP */

