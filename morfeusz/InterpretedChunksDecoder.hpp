/* 
 * File:   InterpsGroupDecoder.hpp
 * Author: mlenart
 *
 * Created on November 22, 2013, 10:35 PM
 */

#ifndef INTERPSGROUPDECODER_HPP
#define	INTERPSGROUPDECODER_HPP

#include <string>
#include <vector>
#include <utility>

#include "charset/CharsetConverter.hpp"
#include "EncodedInterpretation.hpp"
#include "InterpretedChunk.hpp"
#include "EncodedInterpretation.hpp"
#include "charset/CaseConverter.hpp"
#include "Environment.hpp"
#include "MorphInterpretation.hpp"
#include "CasePatternHelper.hpp"
#include "deserializationUtils.hpp"

class InterpretedChunksDecoder {
public:

    InterpretedChunksDecoder(const Environment& env)
    : env(env) {
    }

    virtual ~InterpretedChunksDecoder() {
    }

    virtual void decode(
            unsigned int startNode,
            unsigned int endNode,
            const InterpretedChunk& interpretedChunk,
            std::vector<MorphInterpretation>& out) const = 0;

protected:

    EncodedInterpretation deserializeInterp(const unsigned char*& ptr) const {
        EncodedInterpretation interp;
        deserializeEncodedForm(ptr, interp.value);
        interp.tag = readInt16(ptr);
        interp.nameClassifier = *ptr++;
        interp.qualifiers = readInt16(ptr);
        return interp;
    }

    virtual void deserializeEncodedForm(const unsigned char*& ptr, EncodedForm& encodedForm) const = 0;

    const Environment& env;
};

class InterpretedChunksDecoder4Analyzer : public InterpretedChunksDecoder {
public:

    InterpretedChunksDecoder4Analyzer(const Environment& env) : InterpretedChunksDecoder(env) {
    }

    void decode(
            unsigned int startNode,
            unsigned int endNode,
            const InterpretedChunk& interpretedChunk,
            std::vector<MorphInterpretation>& out) const {
        string orth;
        string lemmaPrefix;
        if (convertPrefixes(interpretedChunk, orth, lemmaPrefix)) {
            orth += env.getCharsetConverter().toString(interpretedChunk.originalCodepoints);
            const unsigned char* currPtr = interpretedChunk.interpsGroup.ptr;
            env.getCasePatternHelper().skipCasePattern(currPtr);
            while (currPtr - interpretedChunk.interpsGroup.ptr < interpretedChunk.interpsGroup.size) {
                this->decodeMorphInterpretation(startNode, endNode, orth, lemmaPrefix, interpretedChunk, currPtr, out);
            }
        }
    }

protected:

    void decodeForm(
            const vector<uint32_t>& orth,
            const EncodedForm& lemma,
            string& res) const {
        for (unsigned int i = 0; i < orth.size() - lemma.suffixToCut; i++) {
            uint32_t cp =
                    (i < lemma.casePattern.size() && lemma.casePattern[i])
                    ? env.getCaseConverter().toTitle(orth[i])
                    : orth[i];
            env.getCharsetConverter().append(cp, res);
        }
        const char* suffixPtr = lemma.suffixToAdd.c_str();
        const char* suffixEnd = suffixPtr + lemma.suffixToAdd.length();
        while (suffixPtr != suffixEnd) {
            uint32_t cp = UTF8CharsetConverter().next(suffixPtr, suffixEnd);
            env.getCharsetConverter().append(cp, res);
        }
    }

    void deserializeEncodedForm(const unsigned char*& ptr, EncodedForm& encodedForm) const {
        encodedForm.suffixToCut = *ptr;
        ptr++;
        encodedForm.suffixToAdd = (const char*) ptr;
        ptr += strlen((const char*) ptr) + 1;
        assert(encodedForm.casePattern.size() == 0);
        env.getCasePatternHelper().deserializeCasePattern(ptr, encodedForm.casePattern);
    }
private:

    pair<string, string> getLemmaHomonymIdPair(const string& lemma) const {
        vector<string> splitRes(split(lemma, ':'));
        if (splitRes.size() == 2) {
            return make_pair(splitRes[0], splitRes[1]);
        } else {
            return make_pair(lemma, "");
        }
    }

    void decodeMorphInterpretation(
            unsigned int startNode, unsigned int endNode,
            const string& orth,
            const string& lemmaPrefix,
            const InterpretedChunk& chunk,
            const unsigned char*& ptr,
            std::vector<MorphInterpretation>& out) const {
        string lemma = lemmaPrefix;
        EncodedInterpretation ei = this->deserializeInterp(ptr);
        if (env.getCasePatternHelper().checkCasePattern(chunk.lowercaseCodepoints, chunk.originalCodepoints, ei.value.casePattern)) {
            this->decodeForm(chunk.lowercaseCodepoints, ei.value, lemma);
            pair<string, string> lemmaHomonymId = getLemmaHomonymIdPair(lemma);
            out.push_back(MorphInterpretation(
                    startNode, endNode,
                    orth, lemmaHomonymId.first,
                    lemmaHomonymId.second,
                    ei.tag,
                    ei.nameClassifier,
                    ei.qualifiers,
                    env));
        }
    }

    bool convertPrefixes(const InterpretedChunk& interpretedChunk, std::string& orth, std::string& lemmaPrefix) const {
        for (unsigned int i = 0; i < interpretedChunk.prefixChunks.size(); i++) {
            const InterpretedChunk& prefixChunk = interpretedChunk.prefixChunks[i];
            orth += env.getCharsetConverter().toString(prefixChunk.originalCodepoints);
            const unsigned char* ptr = prefixChunk.interpsGroup.ptr;
            std::vector<MorphInterpretation> mi;
            env.getCasePatternHelper().skipCasePattern(ptr);
            this->decodeMorphInterpretation(0, 0, orth, string(""), prefixChunk, ptr, mi);
            if (!mi.empty()) {
                lemmaPrefix += mi[0].getLemma();
            } else {
                return false;
            }
        }
        return true;
    }
};

class InterpretedChunksDecoder4Generator : public InterpretedChunksDecoder {
public:

    InterpretedChunksDecoder4Generator(const Environment& env) : InterpretedChunksDecoder(env) {
    }

    void decode(
            unsigned int startNode,
            unsigned int endNode,
            const InterpretedChunk& interpretedChunk,
            std::vector<MorphInterpretation>& out) const {
        string orthPrefix;
        string lemma;
        convertPrefixes(interpretedChunk, orthPrefix, lemma);
        lemma += env.getCharsetConverter().toString(interpretedChunk.originalCodepoints);
        const unsigned char* currPtr = interpretedChunk.interpsGroup.ptr;
        while (currPtr - interpretedChunk.interpsGroup.ptr < interpretedChunk.interpsGroup.size) {
            MorphInterpretation mi = this->decodeMorphInterpretation(startNode, endNode, orthPrefix, lemma, interpretedChunk, currPtr);
            //            cerr << mi.toString(false) << endl;
            //            cerr << "required='" << interpretedChunk.requiredHomonymId << "' morphInterp='" << mi.getHomonymId() << "'" << endl;
            if (interpretedChunk.requiredHomonymId.empty() || mi.getHomonymId() == interpretedChunk.requiredHomonymId) {
                out.push_back(mi);
            }
        }
    }

private:

    void convertPrefixes(const InterpretedChunk& interpretedChunk, std::string& orthPrefix, std::string& lemma) const {
        for (unsigned int i = 0; i < interpretedChunk.prefixChunks.size(); i++) {
            const InterpretedChunk& prefixChunk = interpretedChunk.prefixChunks[i];
            lemma += env.getCharsetConverter().toString(prefixChunk.originalCodepoints);
            const unsigned char* ptr = prefixChunk.interpsGroup.ptr;
            MorphInterpretation mi = this->decodeMorphInterpretation(0, 0, orthPrefix, string(""), prefixChunk, ptr);
            orthPrefix += mi.getOrth();
        }
    }

    MorphInterpretation decodeMorphInterpretation(
            unsigned int startNode, unsigned int endNode,
            const string& orthPrefix,
            const string& lemma,
            const InterpretedChunk& chunk,
            const unsigned char*& ptr) const {
        string orth = orthPrefix;
        string homonymId = (const char*) ptr;
        ptr += strlen((const char*) ptr) + 1;
        EncodedInterpretation ei = this->deserializeInterp(ptr);
        this->decodeForm(chunk.originalCodepoints, ei.value, orth);
        //        string realLemma = homonymId.empty() ? lemma : (lemma + ":" + homonymId);
        return MorphInterpretation(
                startNode, endNode,
                orth, lemma,
                homonymId,
                ei.tag,
                ei.nameClassifier,
                ei.qualifiers,
                env);
    }

    void decodeForm(
            const vector<uint32_t>& lemma,
            const EncodedForm& orth,
            string& res) const {
        res += orth.prefixToAdd;
        for (unsigned int i = 0; i < lemma.size() - orth.suffixToCut; i++) {
            env.getCharsetConverter().append(lemma[i], res);
        }
        const char* suffixPtr = orth.suffixToAdd.c_str();
        const char* suffixEnd = suffixPtr + orth.suffixToAdd.length();
        while (suffixPtr != suffixEnd) {
            uint32_t cp = UTF8CharsetConverter().next(suffixPtr, suffixEnd);
            env.getCharsetConverter().append(cp, res);
        }
    }

    void deserializeEncodedForm(const unsigned char*& ptr, EncodedForm& encodedForm) const {
        encodedForm.prefixToAdd = (const char*) ptr;
        ptr += strlen((const char*) ptr) + 1;
        encodedForm.suffixToCut = *ptr;
        ptr++;
        encodedForm.suffixToAdd = (const char*) ptr;
        ptr += strlen((const char*) ptr) + 1;
    }
};

#endif	/* INTERPSGROUPDECODER_HPP */

