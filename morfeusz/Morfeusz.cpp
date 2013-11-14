/* 
 * File:   Morfeusz.cpp
 * Author: mlenart
 * 
 * Created on November 13, 2013, 5:21 PM
 */

#include <string>
#include "utils.hpp"
#include "Morfeusz.hpp"
#include "MorphDeserializer.hpp"
#include "encoding/CharsetConverter.hpp"

using namespace std;

static FSA<vector<EncodedInterpretation>>* initializeFSA(const string& filename) {
    static Deserializer<vector<EncodedInterpretation>>* deserializer
        = new MorphDeserializer();
    return FSA<vector<EncodedInterpretation>>::getFSA(filename, *deserializer);
}

static CharsetConverter* initializeCharsetConverter() {
    static CharsetConverter* converter = new UTF8CharsetConverter();
    return converter;
}

Morfeusz::Morfeusz(const string& filename)
: fsa(initializeFSA(filename)), charsetConverter(initializeCharsetConverter()) {
    
}

//Morfeusz::Morfeusz(const Morfeusz& orig) {
//}

Morfeusz::~Morfeusz() {
    delete &this->fsa;
}

AnalyzeResult Morfeusz::analyze(const std::string& text) {
    const char* textStart = text.c_str();
    const char* textEnd = text.c_str() + text.length();
    AnalyzeResult res = {
        ResultsIterator(textStart, textEnd, *this),
        ResultsIterator(textEnd, textEnd, *this)};
    return res;
}

