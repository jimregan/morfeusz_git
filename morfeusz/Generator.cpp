/* 
 * File:   Generator.cpp
 * Author: mlenart
 * 
 * Created on 21 styczeń 2014, 14:38
 */

#include <string>
#include <iostream>
#include "charset/charset_utils.hpp"
#include "MorphInterpretation.hpp"
#include "Generator.hpp"
#include "Environment.hpp"


using namespace std;

Generator::Generator(
        const unsigned char* ptr,
        const Environment& env)
: deserializer(env),
fsa(SynthFSAType::getFSA(ptr, deserializer)),
env(env) {
}

Generator::~Generator() {
}

void Generator::appendString(const string& str, string& res) const {
    const char* suffixPtr = str.c_str();
    const char* suffixEnd = suffixPtr + str.length();
    while (suffixPtr != suffixEnd) {
        uint32_t cp = UTF8CharsetConverter().next(suffixPtr, suffixEnd);
        env.getCharsetConverter().append(cp, res);
    }
}

std::string Generator::decodeOrth(
        const EncodedOrth& orth,
        const std::vector<uint32_t>& lemma) const {
    string res;
    this->appendString(orth.prefixToAdd, res);
    for (unsigned int i = 0; i < lemma.size() - orth.suffixToCut; i++) {
        uint32_t cp = lemma[i];
        env.getCharsetConverter().append(cp, res);
    }
    this->appendString(orth.suffixToAdd, res);
//    const char* suffixPtr = orth.suffixToAdd.c_str();
//    const char* suffixEnd = suffixPtr + orth.suffixToAdd.length();
//    while (suffixPtr != suffixEnd) {
//        uint32_t cp = UTF8CharsetConverter().next(suffixPtr, suffixEnd);
//        env.getCharsetConverter().append(cp, res);
//    }
    return res;
}

void Generator::decodeRes(
        const std::vector<EncodedGeneratorInterpretation>& encodedRes,
        const std::string& lemma,
        const std::vector<uint32_t>& lemmaCodepoints,
        std::vector<MorphInterpretation>& result) const {

    for (unsigned int i = 0; i < encodedRes.size(); i++) {
        EncodedGeneratorInterpretation egi = encodedRes[i];
        string decodedOrth = this->decodeOrth(egi.orth, lemmaCodepoints);
        MorphInterpretation mi(
                0, 0,
                decodedOrth, lemma,
                egi.tag,
                egi.nameClassifier,
                env.getAnalyzerTagset(),
                env.getCharsetConverter());
        result.push_back(mi);
    }
}

void Generator::generate(const string& lemma, vector<MorphInterpretation>& result) const {
    const char* currInput = lemma.c_str();
    const char* inputEnd = currInput + lemma.length();
    vector<uint32_t> codepoints;
    SynthStateType state = this->fsa->getInitialState();
    while (currInput != inputEnd && !state.isSink()) {
        uint32_t codepoint = this->env.getCharsetConverter().next(currInput, inputEnd);
        feedState(state, codepoint, this->env.getCharsetConverter());
        codepoints.push_back(codepoint);
    }
    if (state.isAccepting()) {
        vector<EncodedGeneratorInterpretation> encodedRes = state.getValue();
        decodeRes(encodedRes, lemma, codepoints, result);
    }
}
