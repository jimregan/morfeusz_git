/* 
 * File:   Morfeusz.hpp
 * Author: lennyn
 *
 * Created on November 13, 2013, 5:21 PM
 */

#ifndef MORFEUSZ_HPP
#define	MORFEUSZ_HPP

#include <string>
#include <list>
#include <vector>
#include "EncodedInterpretation.hpp"
#include "fsa.hpp"
#include "MorphInterpretation.hpp"
#include "InterpsGroup.hpp"
#include "charset/CharsetConverter.hpp"

class Morfeusz;
//class AnalyzeResult;
class ResultsIterator;

typedef FSA<std::vector<InterpsGroup>> FSAType;
typedef State<std::vector<InterpsGroup>> StateType;

class Morfeusz {
public:
    explicit Morfeusz(const std::string& filename);
    virtual ~Morfeusz();
//    Morfeusz(const Morfeusz& orig);
    ResultsIterator analyze(const std::string& text);

//    Morfeusz();
    friend class ResultsIterator;
private:
    template <class OutputIterator>
//    void processOneWord(const char*& inputData, int startNodeNum, OutputIterator resInterps) const;
    
    int doProcessOneWord(const char*& inputData, int startNodeNum, std::vector<InterpsGroup>& interps) const;
    
    const FSAType* fsa;
    CharsetConverter* charsetConverter;
};

#include "Morfeusz_impl.hpp"

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

