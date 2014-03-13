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
#include "EncodedInterpretation.hpp"
#include "fsa/fsa.hpp"
#include "MorphInterpretation.hpp"
#include "InterpsGroup.hpp"
#include "charset/CharsetConverter.hpp"
#include "charset/CaseConverter.hpp"
#include "InterpretedChunk.hpp"
#include "FlexionGraph.hpp"
#include "MorfeuszOptions.hpp"
#include "const.hpp"
#include "exceptions.hpp"
#include "Generator.hpp"
#include "Environment.hpp"

#include "segrules/segrules.hpp"
#include "segrules/SegrulesFSA.hpp"

class Morfeusz;
class ResultsIterator;

typedef FSA< std::vector<InterpsGroup > > FSAType;
typedef State< std::vector<InterpsGroup > > StateType;

class Morfeusz {
public:
    
    /**
     * Create new instance of Morfeusz class.
     */
    Morfeusz();
    
    /**
     * Set a file used for morphological analysis.
     * 
     * @param filename
     */
    void setAnalyzerFile(const std::string& filename);
    
    /**
     * Set a file used for morphological synthesis.
     * 
     * @param filename
     */
    void setGeneratorFile(const std::string& filename);
    
    virtual ~Morfeusz();
    
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
     * Perform morphological synthesis on a given lemma and put results in a vector.
     * 
     * @param lemma - lemma to be analyzed
     * @param result - results vector
     */
    void generate(const std::string& lemma, std::vector<MorphInterpretation>& result) const;

    /**
     * Set encoding for input and output string objects.
     * 
     * @param encoding
     */
    void setCharset(MorfeuszCharset encoding);

    friend class ResultsIterator;
private:

    void analyzeOneWord(
            const char*& inputData,
            const char* inputEnd,
            int startNodeNum,
            std::vector<MorphInterpretation>& result) const;

    void doAnalyzeOneWord(
            const char*& inputData,
            const char* inputEnd,
            std::vector<InterpretedChunk>& accum,
            FlexionGraph& graph,
            SegrulesState segrulesState) const;

    void appendIgnotiumToResults(
            const std::string& word,
            int startNodeNum,
            std::vector<MorphInterpretation>& results) const;
    Environment env;
    const unsigned char* analyzerPtr;
    FSAType* analyzerFSA;
    std::map<SegrulesOptions, SegrulesFSA*> segrulesFSAsMap;
    SegrulesFSA* currSegrulesFSA;
    bool isAnalyzerFSAFromFile;
    
    const unsigned char* generatorPtr;
    bool isGeneratorFSAFromFile;
    Generator generator;

    MorfeuszOptions options;
};

class ResultsIterator {
public:
    MorphInterpretation getNext();
    bool hasNext();
    friend class Morfeusz;
private:
    ResultsIterator(vector<MorphInterpretation>& res);
    const char* rawInput;
    std::list<MorphInterpretation> resultsBuffer;
    int startNode;
};

#endif	/* MORFEUSZ_HPP */

