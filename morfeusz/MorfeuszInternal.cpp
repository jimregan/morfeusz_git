/* 
 * File:   Morfeusz.cpp
 * Author: mlenart
 * 
 * Created on November 13, 2013, 5:21 PM
 */

#include <string>
#include <iostream>
#include <vector>
#include <cstring>
#include "fsa/fsa.hpp"
#include "utils.hpp"
#include "data/default_fsa.hpp"
#include "MorfeuszInternal.hpp"
#include "deserialization/morphInterps/InterpretedChunksDecoder.hpp"
#include "charset/CharsetConverter.hpp"
#include "charset/charset_utils.hpp"
#include "case/CaseConverter.hpp"
#include "segrules/segrules.hpp"
#include "const.hpp"
#include "charset/utf8.h"
#include "ChunkBounds.hpp"

// TODO - konstruktor kopiujący działający Tak-Jak-Trzeba

using namespace std;

namespace morfeusz {

    static MorfeuszOptions createDefaultOptions() {
        MorfeuszOptions res;
        res.caseHandling = CONDITIONALLY_CASE_SENSITIVE;
        res.encoding = UTF8;
        res.tokenNumbering = SEPARATE_NUMBERING;
        res.whitespaceHandling = SKIP_WHITESPACES;
        res.debug = false;
        return res;
    }

    static void doShiftOrth(InterpretedChunk& from, InterpretedChunk& to) {
        to.prefixChunks.push_back(from);
        to.textStartPtr = from.textStartPtr;
        from.orthWasShifted = true;
    }

    static string debugInterpsGroup(unsigned char type, const char* startPtr, const char* endPtr) {
        stringstream res;
        res << "(" << (int) type << ", " << string(startPtr, endPtr) << "), ";
        return res.str();
    }

    static string debugAccum(vector<InterpretedChunk>& accum) {
        stringstream res;
        for (unsigned int i = 0; i < accum.size(); i++) {
            res << debugInterpsGroup(accum[i].segmentType, accum[i].textStartPtr, accum[i].textEndPtr);
            //        res << "(" << (int) accum[i].interpsGroup.type << ", " << string(accum[i].chunkStartPtr, accum[i].chunkStartPtr) << "), ";
        }
        return res.str();
    }

    static void feedStateDirectly(
            const FSAType& fsa,
            StateType& state,
            const char* inputStart,
            const char* inputEnd) {
        const char* currInput = inputStart;
        while (currInput != inputEnd && !state.isSink()) {
            state.proceedToNext(fsa, *currInput++);
        }
    }

    static void feedStateIndirectly(
            const FSAType& fsa,
            StateType& state,
            uint32_t codepoint) {
        std::string chars;
        UTF8CharsetConverter::getInstance().append(codepoint, chars);
        for (unsigned int i = 0; i < chars.length() && !state.isSink(); i++) {
            state.proceedToNext(fsa, chars[i]);
        }
    }

    static void feedState(
            const Environment& env,
            StateType& state,
            TextReader& reader) {
        if (reader.peek() == reader.normalizedPeek() && &env.getCharsetConverter() == &UTF8CharsetConverter::getInstance()) {
            feedStateDirectly(env.getFSA(), state, reader.getCurrPtr(), reader.getNextPtr());
        }
        else {
            feedStateIndirectly(env.getFSA(), state, reader.normalizedPeek());
        }
    }

    static InterpretedChunk createChunk(
            const InterpsGroup& ig,
            const TextReader& reader,
            bool shiftOrth,
            const string& homonymId) {
        const unsigned char* interpsEndPtr = ig.ptr + ig.size;
        InterpretedChunk ic;
        ic.segmentType = ig.type;
        ic.textStartPtr = reader.getWordStartPtr();
        ic.textEndPtr = homonymId.empty() ? reader.getCurrPtr() : reader.getCurrPtr() - homonymId.length() - 1;
        ic.interpsGroupPtr = ig.ptr;
        ic.interpsEndPtr = interpsEndPtr;
        ic.shiftOrth = shiftOrth;
        ic.orthWasShifted = false;
        ic.requiredHomonymId = homonymId;
        ic.codepointsNum = reader.getCodepointsRead();
        ic.forceIgnoreCase = false;
        return ic;
    }

    MorfeuszInternal::MorfeuszInternal()
    : analyzerEnv(DEFAULT_MORFEUSZ_CHARSET, ANALYZER, DEFAULT_FSA),
    generatorEnv(DEFAULT_MORFEUSZ_CHARSET, GENERATOR, DEFAULT_SYNTH_FSA),
    options(createDefaultOptions()),
    accum(),
    notMatchingCaseSegs(0),
    graph(),
    nextNodeNum(0) {
        analyzerEnv.setCaseSensitive(options.caseHandling != IGNORE_CASE);
        generatorEnv.setCaseSensitive(false);
    }

    void MorfeuszInternal::setAnalyzerDictionary(const string& filename) {
        this->analyzerEnv.setDictionaryFile(filename);
    }

    void MorfeuszInternal::setGeneratorDictionary(const string& filename) {
        this->generatorEnv.setDictionaryFile(filename);
    }

    MorfeuszInternal::~MorfeuszInternal() {
    }

    const char* getWordEndPtr(const TextReader& reader, const Environment& env) {
        TextReader tmpReader(reader.getCurrPtr(), reader.getEndPtr(), env);
        while (!tmpReader.isAtEnd() && !tmpReader.isAtWhitespace()) {
            tmpReader.next();
        }
        return tmpReader.getCurrPtr();
    }

    bool MorfeuszInternal::handleWhitespacesAtBeginning(
            const Environment& env,
            TextReader& reader,
            int startNodeNum,
            std::vector<MorphInterpretation>& results) const {
        if (env.getProcessorType() == ANALYZER) {
            switch (options.whitespaceHandling) {
                case KEEP_WHITESPACES:
                {
                    bool res = reader.isAtWhitespace() && !reader.isAtEnd();
                    if (res) {
                        processWhitespacesChunk(reader, startNodeNum, results);
                    }
                    reader.markChunkStartsHere();
                    reader.markWordStartsHere();
                    return res;
                }
                case APPEND_WHITESPACES:
                    reader.markChunkStartsHere();
                    reader.skipWhitespaces();
                    reader.markWordStartsHere();
                    return false;
                case SKIP_WHITESPACES:
                    reader.skipWhitespaces();
                    reader.markChunkStartsHere();
                    reader.markWordStartsHere();
                    return false;
                default:
                    break;
            }
        }

        return false;
    }

    const char* MorfeuszInternal::handleWhitespacesAtEnd(
            const Environment& env,
            TextReader& reader) const {
        if (env.getProcessorType() == ANALYZER
                && options.whitespaceHandling == APPEND_WHITESPACES) {
            reader.skipWhitespaces();
        }
        return reader.getCurrPtr();
    }

    void MorfeuszInternal::processOneWord(
            const Environment& env,
            TextReader& reader,
            int startNodeNum,
            vector<MorphInterpretation>& results,
            bool insideIgnHandler) const {
        if (handleWhitespacesAtBeginning(env, reader, startNodeNum, results)) {
            startNodeNum = results.back().endNode;
        }

        if (reader.isAtEnd()) {
            return;
        }
        accum.resize(0);
        notMatchingCaseSegs = 0;
        graph.clear();

        const SegrulesFSA& segrulesFSA = env.getCurrentSegrulesFSA();

        doProcessOneWord(env, reader, segrulesFSA.initialState);

        while (reader.isInsideAWord()) {
            reader.next();
        }
        
        ChunkBounds chunkBounds;
        chunkBounds.chunkStartPtr = reader.getChunkStartPtr();
        chunkBounds.wordStartPtr = reader.getWordStartPtr();
        chunkBounds.wordEndPtr = reader.getCurrPtr();
        chunkBounds.chunkEndPtr = handleWhitespacesAtEnd(env, reader);

        if (!graph.empty()) {
            const InterpretedChunksDecoder& interpretedChunksDecoder = env.getInterpretedChunksDecoder();
            int srcNode = startNodeNum;
            const std::vector< std::vector<InflexionGraph::Edge> >& theGraph = graph.getTheGraph();
            size_t initialResultsSize = results.size();
            for (unsigned int i = 0; i < theGraph.size(); i++) {
                const vector<InflexionGraph::Edge>& edges = theGraph[i];
                for (unsigned int j = 0; j < edges.size(); j++) {
                    const InflexionGraph::Edge& e = edges[j];
                    unsigned int targetNode = startNodeNum + e.nextNode;
                    InterpretedChunk ic = e.chunk;
                    ic.chunkStartPtr = 
                            ic.textStartPtr == reader.getWordStartPtr()
                            ? reader.getChunkStartPtr()
                            : ic.textStartPtr;
                    ic.chunkEndPtr = (ic.textEndPtr == chunkBounds.wordEndPtr)
                            ? chunkBounds.wordEndPtr
                            : ic.textEndPtr;
                    interpretedChunksDecoder.decode(srcNode, targetNode, ic, results);
                }
                srcNode++;
            }
            if (results.size() == initialResultsSize) {
                this->appendIgnotiumToResults(env, chunkBounds, startNodeNum, results);
            }
        }
        else if (env.getProcessorType() == ANALYZER
                && !insideIgnHandler) {
            this->handleIgnChunk(env, chunkBounds, startNodeNum, results);
        }
        else {
            this->appendIgnotiumToResults(env, chunkBounds, startNodeNum, results);
        }
    }

    void MorfeuszInternal::doProcessOneWord(
            const Environment& env,
            TextReader& reader,
            const SegrulesState& segrulesState) const {
        if (this->options.debug) {
            cerr << "----------" << endl;
            cerr << "PROCESS: '" << reader.getCurrPtr() << "', already recognized: " << debugAccum(accum) << endl;
        }
        StateType state = env.getFSA().getInitialState();
        string homonymId;
        vector<SegrulesState> newSegrulesStates;
        while (!reader.isAtWhitespace()) {
            feedState(env, state, reader);
            reader.next();
            if (state.isSink()) {
                return;
            }
            if (env.getProcessorType() == GENERATOR && reader.getCurrPtr() != reader.getEndPtr() && reader.peek() == (uint32_t) HOMONYM_SEPARATOR) {
                homonymId = env.getCharsetConverter().fromUTF8(string(reader.getCurrPtr() + 1, reader.getEndPtr()));
                reader.proceedToEnd();
            }
            if (state.isAccepting()) {
                InterpsGroupsReader& igReader = const_cast<InterpsGroupsReader&> (state.getValue());
                while (igReader.hasNext()) {
                    processInterpsGroup(env, reader, reader.isAtWhitespace(), segrulesState, homonymId, igReader.getNext(), newSegrulesStates);
                }
            }
        }
    }

    void MorfeuszInternal::processInterpsGroup(
            const Environment& env,
            const TextReader& reader,
            bool isAtWhitespace,
            const SegrulesState& segrulesState,
            const string& homonymId,
            const InterpsGroup& ig,
            vector<SegrulesState>& newSegrulesStates) const {
        bool caseMatches = env.getCasePatternHelper().checkInterpsGroupOrthCasePatterns(env, reader.getWordStartPtr(), reader.getCurrPtr(), ig);
        if (caseMatches || options.caseHandling == CONDITIONALLY_CASE_SENSITIVE) {

            env.getCurrentSegrulesFSA().proceedToNext(ig.type, segrulesState, isAtWhitespace, newSegrulesStates);
            if (!newSegrulesStates.empty()) {
                for (unsigned int i = 0; i < newSegrulesStates.size(); i++) {
                    const SegrulesState& newSegrulesState = newSegrulesStates[i];

                    InterpretedChunk ic(
                            createChunk(ig, reader, newSegrulesState.shiftOrthFromPrevious, homonymId));

                    processInterpretedChunk(
                            env,
                            reader,
                            isAtWhitespace,
                            caseMatches,
                            newSegrulesState,
                            ic);
                }
                newSegrulesStates.resize(0);
            }
            else if (this->options.debug) {
                std::cerr << "NOT ACCEPTING (segmentation)" << debugAccum(accum) << debugInterpsGroup(ig.type, reader.getWordStartPtr(), reader.getCurrPtr()) << std::endl;
            }
        }
        else if (this->options.debug) {
            std::cerr << "NOT ACCEPTING (case)" << debugAccum(accum) << debugInterpsGroup(ig.type, reader.getWordStartPtr(), reader.getCurrPtr()) << std::endl;
        }
    }

    void MorfeuszInternal::processInterpretedChunk(
            const Environment& env,
            const TextReader& reader,
            bool isAtWhitespace,
            bool caseMatches,
            const SegrulesState& newSegrulesState,
            InterpretedChunk& ic) const {
        if (!accum.empty() && accum.back().shiftOrth) {
            doShiftOrth(accum.back(), ic);
        }
        if (!caseMatches && options.caseHandling == CONDITIONALLY_CASE_SENSITIVE) {
            notMatchingCaseSegs++;
            ic.forceIgnoreCase = true;
        }
        accum.push_back(ic);
        if (isAtWhitespace) {
            assert(newSegrulesState.accepting);
            if (this->options.debug) {
                cerr << "ACCEPTING " << debugAccum(accum) << endl;
            }
            graph.addPath(accum, newSegrulesState.weak || notMatchingCaseSegs > 0);
        }
        else {
            assert(!newSegrulesState.sink);
            TextReader newReader(reader.getCurrPtr(), reader.getEndPtr(), env);
            doProcessOneWord(env, newReader, newSegrulesState);
        }
        accum.pop_back();
        if (!caseMatches && options.caseHandling == CONDITIONALLY_CASE_SENSITIVE) {
            notMatchingCaseSegs--;
        }
    }

    void MorfeuszInternal::processWhitespacesChunk(
            TextReader& reader,
            int startNodeNum,
            std::vector<MorphInterpretation>& results) const {
        string orth(reader.readWhitespacesChunk());
        results.push_back(MorphInterpretation::createWhitespace(startNodeNum, startNodeNum + 1, orth));
    }

    void MorfeuszInternal::handleIgnChunk(
            const Environment& env,
            const ChunkBounds& chunkBounds,
            int startNodeNum,
            std::vector<MorphInterpretation>& results) const {
        const char* currInput = chunkBounds.chunkStartPtr;
        const char* prevInput;
        uint32_t codepoint = 0x00;
        bool separatorFound = false;
        while (currInput != chunkBounds.chunkEndPtr) {
            prevInput = currInput;
            const char* nonSeparatorInputEnd = prevInput;
            do {
                codepoint = env.getCharsetConverter().next(currInput, chunkBounds.chunkEndPtr);
                if (!env.isSeparator(codepoint)) {
                    nonSeparatorInputEnd = currInput;
                }
            }
            while (currInput != chunkBounds.chunkEndPtr && !env.isSeparator(codepoint));

            if (env.isSeparator(codepoint)) {
                separatorFound = true;
                if (nonSeparatorInputEnd != prevInput) {
                    // there are non-separators + separators

                    int startNode = results.empty() ? startNodeNum : results.back().endNode;
                    // process part before separators
                    TextReader newReader1(prevInput, nonSeparatorInputEnd, env);
                    notMatchingCaseSegs = 0;
                    this->processOneWord(env, newReader1, startNode, results, true);

                    // process separators part
                    if (currInput == chunkBounds.wordEndPtr) {
                        currInput = chunkBounds.chunkEndPtr;
                    }
                    startNode = results.empty() ? startNodeNum : results.back().endNode;
                    TextReader newReader2(nonSeparatorInputEnd, currInput, env);
                    this->processOneWord(env, newReader2, startNode, results, true);
                }
                else {
                    // there are only separators
                    if (currInput == chunkBounds.wordEndPtr) {
                        currInput = chunkBounds.chunkEndPtr;
                    }
                    int startNode = results.empty() ? startNodeNum : results.back().endNode;
                    TextReader newReader3(prevInput, currInput, env);
                    notMatchingCaseSegs = 0;
                    this->processOneWord(env, newReader3, startNode, results, true);
                }
            }
        }

        // currInput == chunkBounds.chunkEndPtr
        if (!env.isSeparator(codepoint)) {
            if (separatorFound) {
                // process part after separators
                int startNode = results.empty() ? startNodeNum : results.back().endNode;
                TextReader newReader4(prevInput, chunkBounds.chunkEndPtr, env);
                this->processOneWord(env, newReader4, startNode, results, true);
            }
            else {
                this->appendIgnotiumToResults(env, chunkBounds, startNodeNum, results);
            }
        }
    }

    void MorfeuszInternal::appendIgnotiumToResults(
            const Environment& env,
            const ChunkBounds& chunkBounds,
            int startNodeNum,
            std::vector<MorphInterpretation>& results) const {
        string orth(chunkBounds.chunkStartPtr, chunkBounds.chunkEndPtr);
        string lemma(chunkBounds.wordStartPtr, chunkBounds.wordEndPtr);
        results.push_back(MorphInterpretation::createIgn(startNodeNum, startNodeNum + 1, orth, lemma));
    }

    void MorfeuszInternal::analyseOneWord(
            TextReader& reader,
            vector<MorphInterpretation>& results) const {
        this->processOneWord(this->analyzerEnv, reader, nextNodeNum, results);
        if (!results.empty()) {
            nextNodeNum = results.back().endNode;
        }
    }

    void MorfeuszInternal::adjustTokensCounter() const {
        if (options.tokenNumbering == SEPARATE_NUMBERING) {
            nextNodeNum = 0;
        }
    }

    ResultsIterator* MorfeuszInternal::analyse(const string& text) const {
        adjustTokensCounter();
        char* textCopy = new char[text.length() + 1];
        strcpy(textCopy, text.c_str());
        return new ResultsIteratorImpl(*this, textCopy, textCopy + text.length(), true);
    }
    
    ResultsIterator* MorfeuszInternal::analyseWithCopy(const char* text) const {
        adjustTokensCounter();
        long n = strlen(text);
        char* textCopy = new char[n + 1];
        strcpy(textCopy, text);
        return new ResultsIteratorImpl(*this, textCopy, textCopy + n, true);
    }

    ResultsIterator* MorfeuszInternal::analyse(const char* text) const {
        adjustTokensCounter();
        return new ResultsIteratorImpl(*this, text, text + strlen(text), false);
    }

    void MorfeuszInternal::analyse(const string& text, vector<MorphInterpretation>& results) const {
        adjustTokensCounter();
        TextReader reader(text, this->analyzerEnv);
        while (!reader.isAtEnd()) {
            analyseOneWord(reader, results);
        }
    }

    void MorfeuszInternal::generate(const string& lemma, vector<MorphInterpretation>& results) const {
        const char* input = lemma.c_str();
        const char* inputEnd = input + lemma.length();
        int startNode = 0;
        TextReader reader(input, inputEnd, this->generatorEnv);
        this->processOneWord(this->generatorEnv, reader, startNode, results);
        if (reader.getCurrPtr() != reader.getEndPtr()) {
            throw MorfeuszException("Input contains more than one word");
        }
    }

    void MorfeuszInternal::generate(const std::string& lemma, int tagId, vector<MorphInterpretation>& result) const {
        vector<MorphInterpretation> partRes;
        this->generate(lemma, partRes);
        for (unsigned int i = 0; i < partRes.size(); i++) {
            // XXX - someday it should be improved
            if (partRes[i].tagId == tagId) {
                result.push_back(partRes[i]);
            }
        }
    }

    void MorfeuszInternal::setCharset(Charset charset) {
        this->options.encoding = charset;
        this->analyzerEnv.setCharset(charset);
        this->generatorEnv.setCharset(charset);
    }

    void MorfeuszInternal::setAggl(const std::string& aggl) {
        this->analyzerEnv.setSegrulesOption("aggl", aggl);
        this->generatorEnv.setSegrulesOption("aggl", aggl);
    }

    void MorfeuszInternal::setPraet(const std::string& praet) {
        this->analyzerEnv.setSegrulesOption("praet", praet);
        this->generatorEnv.setSegrulesOption("praet", praet);
    }

    void MorfeuszInternal::setCaseHandling(CaseHandling caseHandling) {
        this->analyzerEnv.setCaseSensitive(caseHandling != IGNORE_CASE);
    }

    void MorfeuszInternal::setTokenNumbering(TokenNumbering tokenNumbering) {
        this->options.tokenNumbering = tokenNumbering;
        nextNodeNum = 0;
    }

    void MorfeuszInternal::setWhitespaceHandling(WhitespaceHandling whitespaceHandling) {
        this->options.whitespaceHandling = whitespaceHandling;
    }

    void MorfeuszInternal::setDebug(bool debug) {
        this->options.debug = debug;
    }

    const IdResolver& MorfeuszInternal::getDefaultAnalyzerTagset() const {
        return this->generatorEnv.getTagset();
    }

    const IdResolver& MorfeuszInternal::getDefaultGeneratorTagset() const {
        return this->analyzerEnv.getTagset();
    }
    
    const IdResolver& MorfeuszInternal::getIdResolver() const {
        return this->analyzerEnv.getTagset();
    }
}
