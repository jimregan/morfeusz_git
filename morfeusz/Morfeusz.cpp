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
#include "GeneratorDeserializer.hpp"
#include "InterpretedChunksDecoder.hpp"
#include "charset/CharsetConverter.hpp"
#include "charset/charset_utils.hpp"
#include "charset/CaseConverter.hpp"
#include "const.hpp"

// TODO - konstruktor kopiujący działający Tak-Jak-Trzeba

using namespace std;

static Deserializer<vector<InterpsGroup> >* initializeAnalyzerDeserializer() {
    static Deserializer < vector < InterpsGroup > > *deserializer
            = new MorphDeserializer();
    return deserializer;
}

static FSA<vector<InterpsGroup > > *initializeAnalyzerFSA(const string& filename) {
    cerr << "initialize FSA" << endl;
    return FSA < vector < InterpsGroup > > ::getFSA(filename, *initializeAnalyzerDeserializer());
}

//static FSA<vector<MorphInterpretation > > *initializeSynthFSA(const string& filename, const SynthDeserializer& deserializer) {
//    cerr << "initialize synth FSA" << endl;
//    return FSA < vector < EncodedGeneratorInterpretation > > ::getFSA(filename, deserializer);
//}
//
//static CharsetConverter* getCharsetConverter(MorfeuszCharset charset) {
//    cerr << "initialize charset converter for " << charset << endl;
//    static CharsetConverter* utf8Converter = new UTF8CharsetConverter();
////    static CharsetConverter* utf16LEConverter = new UTF16CharsetConverter(UTF16CharsetConverter::UTF16CharsetConverter::LE);
////    static CharsetConverter* utf16BEConverter = new UTF16CharsetConverter(UTF16CharsetConverter::Endianness::BE);
//    static CharsetConverter* iso8859_2Converter = new ISO8859_2_CharsetConverter();
//    static CharsetConverter* windows1250Converter = new Windows_1250_CharsetConverter();
//    static CharsetConverter* cp852Converter = new CP852_CharsetConverter();
//    switch (charset) {
//        case UTF8:
//            return utf8Converter;
//        case ISO8859_2:
//            return iso8859_2Converter;
//        case CP1250:
//            return windows1250Converter;
//        case CP852:
//            return cp852Converter;
//        default:
//            throw MorfeuszException("invalid charset");
//    }
//}
//
//static Tagset* initializeTagset(const string& filename) {
//    cerr << "initialize tagset" << endl;
//    static Tagset* tagset = new Tagset(readFile<unsigned char>(filename.c_str()));
//    return tagset;
//}
//
//static Tagset* initializeTagset(const unsigned char* data) {
//    cerr << "initialize tagset" << endl;
//    static Tagset* tagset = new Tagset(data);
//    return tagset;
//}
//
//static CaseConverter* initializeCaseConverter() {
//    cerr << "initialize case converter" << endl;
//    static CaseConverter* cc = new CaseConverter();
//    return cc;
//}

static MorfeuszOptions createDefaultOptions() {
    MorfeuszOptions res;
    res.caseSensitive = true;
    res.encoding = UTF8;
    return res;
}

Morfeusz::Morfeusz()
: env(Tagset(DEFAULT_FSA), Tagset(DEFAULT_SYNTH_FSA), DEFAULT_MORFEUSZ_CHARSET),
analyzerFSA(FSAType::getFSA(DEFAULT_FSA, *initializeAnalyzerDeserializer())),
isAnalyzerFSAFromFile(false),
generator(DEFAULT_SYNTH_FSA, env),
options(createDefaultOptions()) {

}

void Morfeusz::setAnalyzerFile(const string& filename) {
    if (this->isAnalyzerFSAFromFile) {
        delete this->analyzerFSA;
    }
    this->analyzerFSA = initializeAnalyzerFSA(filename);
    this->isAnalyzerFSAFromFile = true;
}

Morfeusz::~Morfeusz() {
    if (this->isAnalyzerFSAFromFile) {
        delete this->analyzerFSA;
    }
}

void Morfeusz::analyzeOneWord(
        const char*& inputData,
        const char* inputEnd,
        int startNodeNum,
        std::vector<MorphInterpretation>& results) const {
    while (inputData != inputEnd
            && isEndOfWord(this->env.getCharsetConverter().peek(inputData, inputEnd))) {
        this->env.getCharsetConverter().next(inputData, inputEnd);
    }
    const char* wordStart = inputData;
    vector<InterpretedChunk> accum;
    FlexionGraph graph;
    const char* currInput = inputData;
    doAnalyzeOneWord(currInput, inputEnd, accum, graph);
    if (!graph.empty()) {
        InterpretedChunksDecoder interpretedChunksDecoder(env);
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

void Morfeusz::doAnalyzeOneWord(
        const char*& inputData,
        const char* inputEnd,
        vector<InterpretedChunk>& accum,
        FlexionGraph& graph) const {
    bool endOfWord = inputData == inputEnd;
    const char* currInput = inputData;
    uint32_t codepoint = endOfWord ? 0 : this->env.getCharsetConverter().next(currInput, inputEnd);
    //    UnicodeChunk uchunk(*(this->charsetConverter), *(this->caseConverter));
    vector<uint32_t> originalCodepoints;
    vector<uint32_t> lowercaseCodepoints;

    StateType state = this->analyzerFSA->getInitialState();

    while (!isEndOfWord(codepoint)) {
        uint32_t lowerCP = this->env.getCaseConverter().toLower(codepoint);
        originalCodepoints.push_back(codepoint);
        lowercaseCodepoints.push_back(lowerCP);
        feedState(state, lowerCP, UTF8CharsetConverter());
        if (state.isAccepting()) {
            vector< InterpsGroup > val(state.getValue());
            for (unsigned int i = 0; i < val.size(); i++) {
                InterpsGroup& ig = val[i];
                InterpretedChunk ic = {inputData, originalCodepoints, lowercaseCodepoints, ig};
                accum.push_back(ic);
                const char* newCurrInput = currInput;
                doAnalyzeOneWord(newCurrInput, inputEnd, accum, graph);
                accum.pop_back();
            }
        }
        codepoint = currInput == inputEnd ? 0 : this->env.getCharsetConverter().peek(currInput, inputEnd);
        if (!isEndOfWord(codepoint)) {
            this->env.getCharsetConverter().next(currInput, inputEnd);
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

void Morfeusz::appendIgnotiumToResults(
        const string& word,
        int startNodeNum,
        std::vector<MorphInterpretation>& results) const {
    MorphInterpretation interp = MorphInterpretation::createIgn(startNodeNum, word, env.getAnalyzerTagset(), env.getCharsetConverter());
    results.push_back(interp);
}

ResultsIterator Morfeusz::analyze(const string& text) const {
    //    const char* textStart = text.c_str();
    //    const char* textEnd = text.c_str() + text.length();
    vector<MorphInterpretation> res;
    this->analyze(text, res);
    return ResultsIterator(res);
}

void Morfeusz::analyze(const string& text, vector<MorphInterpretation>& results) const {
    const char* input = text.c_str();
    const char* inputEnd = input + text.length();
    while (input != inputEnd) {
        int startNode = results.empty() ? 0 : results.back().getEndNode();
        this->analyzeOneWord(input, inputEnd, startNode, results);
    }
}

ResultsIterator Morfeusz::generate(const string& text) const {
    //    const char* textStart = text.c_str();
    //    const char* textEnd = text.c_str() + text.length();
    vector<MorphInterpretation> res;
    this->generate(text, res);
    return ResultsIterator(res);
}

void Morfeusz::generate(const string& text, vector<MorphInterpretation>& results) const {
    this->generator.generate(text, results);
}

void Morfeusz::setCharset(MorfeuszCharset charset) {
    this->options.encoding = charset;
    this->env.setCharset(charset);
}

ResultsIterator::ResultsIterator(vector<MorphInterpretation>& res) {
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
