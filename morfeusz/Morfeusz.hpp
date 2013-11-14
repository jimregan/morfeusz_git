/* 
 * File:   Morfeusz.hpp
 * Author: lennyn
 *
 * Created on November 13, 2013, 5:21 PM
 */

#ifndef MORFEUSZ_HPP
#define	MORFEUSZ_HPP

#include <string>
#include <vector>
#include "EncodedInterpretation.hpp"
#include "fsa.hpp"
#include "MorphInterpretation.hpp"
#include "encoding/CharsetConverter.hpp"

class Morfeusz;
class AnalyzeResult;
class ResultsIterator;

class Morfeusz {
public:
    explicit Morfeusz(const std::string& filename);
    virtual ~Morfeusz();
//    Morfeusz(const Morfeusz& orig);
    AnalyzeResult analyze(const std::string& text);

//    Morfeusz();
private:
    void processOneWord(const char*& inputData, int startNodeNum, std::vector<MorphInterpretation>& resInterps);
    const FSA<std::vector<EncodedInterpretation>>* fsa;
    CharsetConverter* charsetConverter;
};

class ResultsIterator {
public:
    ResultsIterator(
                const char* startOfInput,
                const char* endOfInput,
                const Morfeusz& morfeusz);
    virtual ~ResultsIterator();
//    ResultsIterator(int* x);
    ResultsIterator(const ResultsIterator& mit);
    ResultsIterator& operator++();
    ResultsIterator operator++(int);
    bool operator==(const ResultsIterator& rhs);
    bool operator!=(const ResultsIterator& rhs);
    MorphInterpretation& operator*();
private:
    const char* rawInput;
    const char* endOfInput;
};

struct AnalyzeResult {
    ResultsIterator iterator;
    const ResultsIterator end;
};

#endif	/* MORFEUSZ_HPP */

