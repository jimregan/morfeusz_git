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

class Morfeusz;
class ResultsIterator;

typedef FSA<std::vector<InterpsGroup > > FSAType;
typedef State<std::vector<InterpsGroup > > StateType;

class MorfeuszException : public std::exception {
public:

    MorfeuszException(const std::string& what) : msg(what.c_str()) {
    }

    virtual ~MorfeuszException() throw () {
    }

    virtual const char* what() const throw () {
        return this->msg.c_str();
    }
private:
    const std::string msg;
};

class Morfeusz {
public:
    Morfeusz();
    explicit Morfeusz(const std::string& filename);
    virtual ~Morfeusz();
    //    Morfeusz(const Morfeusz& orig);
    ResultsIterator analyze(const std::string& text) const;
    void analyze(const std::string& text, std::vector<MorphInterpretation>& result) const;

    void setEncoding(MorfeuszCharset encoding);

    //    Morfeusz();
    friend class ResultsIterator;
private:

    void processOneWord(
            const char*& inputData,
            const char* inputEnd,
            int startNodeNum,
            std::vector<MorphInterpretation>& result) const;

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

    UTF8CharsetConverter utf8CharsetConverter;

    MorfeuszOptions options;
};

class ResultsIterator {
public:
    MorphInterpretation getNext();
    bool hasNext();
    friend class Morfeusz;
private:
    ResultsIterator(const std::string& text, const Morfeusz& morfeusz);
    const char* rawInput;
    const Morfeusz& morfeusz;
    std::list<MorphInterpretation> resultsBuffer;
    int startNode;
};

#endif	/* MORFEUSZ_HPP */

