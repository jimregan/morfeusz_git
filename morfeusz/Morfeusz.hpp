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

class Morfeusz;
//class AnalyzeResult;
class ResultsIterator;

typedef FSA<std::vector<InterpsGroup >> FSAType;
typedef State<std::vector<InterpsGroup >> StateType;

class Morfeusz {
public:
    Morfeusz();
    explicit Morfeusz(const std::string& filename);
    virtual ~Morfeusz();
    //    Morfeusz(const Morfeusz& orig);
    ResultsIterator analyze(const std::string& text) const;
    void analyze(const std::string& text, std::vector<MorphInterpretation>& result) const;

    void processOneWord(
            const char*& inputData,
            const char* inputEnd,
            int startNodeNum,
            std::vector<MorphInterpretation>& result) const;

    //    Morfeusz();
    friend class ResultsIterator;
private:

    void doProcessOneWord(
            const char*& inputData,
            const char* inputEnd,
            std::vector<InterpretedChunk>& accum,
            FlexionGraph& graph) const;

    void feedState(
            StateType& state,
            int codepoint) const;

    void appendIgnotiumToResults(
            const std::string& word,
            int startNodeNum,
            std::vector<MorphInterpretation>& results) const;

    FSAType* fsa;
    CharsetConverter* charsetConverter;
    Tagset* tagset;
    CaseConverter* caseConverter;

    bool caseSensitive;
};

class ResultsIterator {
public:
    ResultsIterator(const std::string& text, const Morfeusz& morfeusz);
    MorphInterpretation getNext();
    bool hasNext();
private:
    const char* rawInput;
    const Morfeusz& morfeusz;
    std::list<MorphInterpretation> resultsBuffer;
    int startNode;
};

//class ResultsIterator {
//public:
//    ResultsIterator(
//                const char* startOfInput,
//                const char* endOfInput,
//                const Morfeusz& morfeusz);
//    virtual ~ResultsIterator();
//    ResultsIterator(const ResultsIterator& mit);
//    ResultsIterator& operator++();
//    ResultsIterator operator++(int);
//    bool operator==(const ResultsIterator& rhs);
//    bool operator!=(const ResultsIterator& rhs);
//    MorphInterpretation& operator*();
//private:
//    const char* rawInput;
//    const char* endOfInput;
//    const Morfeusz& morfeusz;
//    vector<MorphInterpretation> resultsBuffer;
//};

//struct AnalyzeResult {
//    ResultsIterator iterator;
//    const ResultsIterator end;
//};

#endif	/* MORFEUSZ_HPP */

