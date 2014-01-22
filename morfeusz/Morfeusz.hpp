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

class Morfeusz;
class ResultsIterator;

typedef FSA< std::vector<InterpsGroup > > FSAType;
typedef State< std::vector<InterpsGroup > > StateType;

class Morfeusz {
public:
    Morfeusz();
    //    explicit Morfeusz(const std::string& filename);
    void setAnalyzerFile(const std::string& filename);
    void setSynthesizerFile(const std::string& filename);
    virtual ~Morfeusz();
    //    Morfeusz(const Morfeusz& orig);
    ResultsIterator analyze(const std::string& text) const;
    void analyze(const std::string& text, std::vector<MorphInterpretation>& result) const;

    void generate(const std::string& lemma, std::vector<MorphInterpretation>& result) const;
    ResultsIterator generate(const std::string& lemma) const;

    void setCharset(MorfeuszCharset encoding);

    //    Morfeusz();
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
            FlexionGraph& graph) const;

    void appendIgnotiumToResults(
            const std::string& word,
            int startNodeNum,
            std::vector<MorphInterpretation>& results) const;
    Environment env;
    FSAType* analyzerFSA;
    bool isAnalyzerFSAFromFile;
    Generator generator;
//    const CharsetConverter* charsetConverter;
//    const Tagset* tagset;
//    const CaseConverter* caseConverter;
//
//    UTF8CharsetConverter utf8CharsetConverter;

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

