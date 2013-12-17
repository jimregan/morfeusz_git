/* 
 * File:   Morfeusz.cpp
 * Author: mlenart
 * 
 * Created on November 13, 2013, 5:21 PM
 */

#include <string>
#include <iostream>
#include "fsa/fsa.hpp"
#include "utils.hpp"
#include "data/default_fsa.hpp"
#include "Morfeusz.hpp"
#include "MorphDeserializer.hpp"
#include "InterpretedChunksDecoder.hpp"
#include "charset/CharsetConverter.hpp"
#include "charset/charset_utils.hpp"
#include "charset/CaseConverter.hpp"
#include "const.hpp"

// TODO - konstruktor kopiujący działający Tak-Jak-Trzeba

using namespace std;

static Deserializer<vector<InterpsGroup> >* initializeDeserializer() {
    static Deserializer < vector < InterpsGroup > > *deserializer
            = new MorphDeserializer();
    return deserializer;
}

static FSA<vector<InterpsGroup > > *initializeFSA(const string& filename) {
    cerr << "initialize FSA" << endl;
    return FSA < vector < InterpsGroup > > ::getFSA(filename, *initializeDeserializer());
}

static CharsetConverter* getCharsetConverter(MorfeuszCharset charset) {
    cerr << "initialize charset converter for " << charset << endl;
    static CharsetConverter* utf8Converter = new UTF8CharsetConverter();
//    static CharsetConverter* utf16LEConverter = new UTF16CharsetConverter(UTF16CharsetConverter::UTF16CharsetConverter::LE);
//    static CharsetConverter* utf16BEConverter = new UTF16CharsetConverter(UTF16CharsetConverter::Endianness::BE);
    static CharsetConverter* iso8859_2Converter = new ISO8859_2_CharsetConverter();
    static CharsetConverter* windows1250Converter = new Windows_1250_CharsetConverter();
    static CharsetConverter* cp852Converter = new CP852_CharsetConverter();
    switch (charset) {
        case UTF8:
            return utf8Converter;
        case ISO8859_2:
            return iso8859_2Converter;
        case CP1250:
            return windows1250Converter;
        case CP852:
            return cp852Converter;
        default:
            throw MorfeuszException("invalid charset");
    }
}

static Tagset* initializeTagset(const string& filename) {
    cerr << "initialize tagset" << endl;
    static Tagset* tagset = new Tagset(readFile<unsigned char>(filename.c_str()));
    return tagset;
}

static Tagset* initializeTagset(const unsigned char* data) {
    cerr << "initialize tagset" << endl;
    static Tagset* tagset = new Tagset(data);
    return tagset;
}

static CaseConverter* initializeCaseConverter() {
    cerr << "initialize case converter" << endl;
    static CaseConverter* cc = new CaseConverter();
    return cc;
}

static MorfeuszOptions createDefaultOptions() {
    MorfeuszOptions res;
    res.caseSensitive = true;
    res.encoding = UTF8;
    return res;
}

Morfeusz::Morfeusz()
: fsa(FSAType::getFSA(DEFAULT_FSA, *initializeDeserializer())),
charsetConverter(getCharsetConverter(DEFAULT_MORFEUSZ_CHARSET)),
tagset(initializeTagset(DEFAULT_FSA)),
caseConverter(initializeCaseConverter()),
options(createDefaultOptions()) {

}

Morfeusz::Morfeusz(const string& filename)
: fsa(initializeFSA(filename)),
charsetConverter(getCharsetConverter(DEFAULT_MORFEUSZ_CHARSET)),
tagset(initializeTagset(filename)),
caseConverter(initializeCaseConverter()),
options(createDefaultOptions()) {

}

Morfeusz::~Morfeusz() {
    //    delete &this->fsa;
    //    delete &this->charsetConverter;
}

void Morfeusz::processOneWord(
        const char*& inputData,
        const char* inputEnd,
        int startNodeNum,
        std::vector<MorphInterpretation>& results) const {
    while (inputData != inputEnd
            && isEndOfWord(this->charsetConverter->peek(inputData, inputEnd))) {
        this->charsetConverter->next(inputData, inputEnd);
    }
    const char* wordStart = inputData;
    vector<InterpretedChunk> accum;
    FlexionGraph graph;
    const char* currInput = inputData;
    doProcessOneWord(currInput, inputEnd, accum, graph);
    if (!graph.empty()) {
        InterpretedChunksDecoder interpretedChunksDecoder(*tagset, *charsetConverter, *caseConverter);
        int srcNode = startNodeNum;
        for (unsigned int i = 0; i < graph.getTheGraph().size(); i++) {
            vector<FlexionGraph::Edge>& edges = graph.getTheGraph()[i];
            for (unsigned int j = 0; j < edges.size(); j++) {
                FlexionGraph::Edge& e = edges[j];
                int targetNode = startNodeNum + e.nextNode;
                interpretedChunksDecoder.decode(srcNode, targetNode, e.chunk, back_inserter(results));
            }
            srcNode++;
        }
        //        graph.getResults(*this->tagset, results);
    } else if (wordStart != currInput) {
        this->appendIgnotiumToResults(string(wordStart, currInput), startNodeNum, results);
    }
    inputData = currInput;
}

void Morfeusz::doProcessOneWord(
        const char*& inputData,
        const char* inputEnd,
        vector<InterpretedChunk>& accum,
        FlexionGraph& graph) const {
    bool endOfWord = inputData == inputEnd;
    const char* currInput = inputData;
    uint32_t codepoint = endOfWord ? 0 : this->charsetConverter->next(currInput, inputEnd);
    //    UnicodeChunk uchunk(*(this->charsetConverter), *(this->caseConverter));
    vector<uint32_t> originalCodepoints;
    vector<uint32_t> lowercaseCodepoints;

    StateType state = this->fsa->getInitialState();

    while (!isEndOfWord(codepoint)) {
        uint32_t lowerCP = this->caseConverter->toLower(codepoint);
        originalCodepoints.push_back(codepoint);
        lowercaseCodepoints.push_back(lowerCP);
        this->feedState(state, lowerCP);
        if (state.isAccepting()) {
            vector< InterpsGroup > val(state.getValue());
            for (unsigned int i = 0; i < val.size(); i++) {
                InterpsGroup& ig = val[i];
                InterpretedChunk ic = {inputData, originalCodepoints, lowercaseCodepoints, ig};
                accum.push_back(ic);
                const char* newCurrInput = currInput;
                doProcessOneWord(newCurrInput, inputEnd, accum, graph);
                accum.pop_back();
            }
        }
        codepoint = currInput == inputEnd ? 0 : this->charsetConverter->peek(currInput, inputEnd);
        if (!isEndOfWord(codepoint)) {
            this->charsetConverter->next(currInput, inputEnd);
        }
    }
    if (state.isAccepting()) {
        vector<InterpsGroup > val(state.getValue());
        for (unsigned int i = 0; i < val.size(); i++) {
            InterpsGroup& ig = val[i];
            InterpretedChunk ic = {inputData, originalCodepoints, lowercaseCodepoints, ig};
            accum.push_back(ic);
            graph.addPath(accum);
            accum.pop_back();
        }
    }
    inputData = currInput;
}

void Morfeusz::feedState(
        StateType& state,
        int codepoint) const {
    string chars;
    this->utf8CharsetConverter.append(codepoint, chars);
    for (unsigned int i = 0; i < chars.length(); i++) {
        state.proceedToNext(chars[i]);
    }
}

void Morfeusz::appendIgnotiumToResults(
        const string& word,
        int startNodeNum,
        std::vector<MorphInterpretation>& results) const {
    MorphInterpretation interp = MorphInterpretation::createIgn(startNodeNum, word, *this->tagset, *this->charsetConverter);
    results.push_back(interp);
}

ResultsIterator Morfeusz::analyze(const string& text) const {
    //    const char* textStart = text.c_str();
    //    const char* textEnd = text.c_str() + text.length();
    return ResultsIterator(text, *this);
}

void Morfeusz::analyze(const string& text, vector<MorphInterpretation>& results) const {
    const char* input = text.c_str();
    const char* inputEnd = input + text.length();
    while (input != inputEnd) {
        int startNode = results.empty() ? 0 : results.back().getEndNode();
        DEBUG("process " + string(input, inputEnd));
        this->processOneWord(input, inputEnd, startNode, results);
    }
}

void Morfeusz::setEncoding(MorfeuszCharset encoding) {
    this->options.encoding = encoding;
    this->charsetConverter = getCharsetConverter(encoding);
}

ResultsIterator::ResultsIterator(const string& text, const Morfeusz& morfeusz)
: rawInput(text.c_str()),
morfeusz(morfeusz) {
    vector<MorphInterpretation> res;
    morfeusz.analyze(text, res);
    resultsBuffer.insert(resultsBuffer.begin(), res.begin(), res.end());
}

MorphInterpretation ResultsIterator::getNext() {
    MorphInterpretation res = this->resultsBuffer.front();
    this->resultsBuffer.pop_front();
    return res;
}

bool ResultsIterator::hasNext() {
    return !resultsBuffer.empty();
}
