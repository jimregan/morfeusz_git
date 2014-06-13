/* 
 * File:   Environment.cpp
 * Author: mlenart
 * 
 * Created on 22 styczeń 2014, 12:08
 */

#include <vector>
#include <algorithm>
#include "Environment.hpp"
#include "deserialization/MorphDeserializer.hpp"
#include "exceptions.hpp"
#include "deserialization/morphInterps/InterpretedChunksDecoder.hpp"
#include "deserialization/morphInterps/InterpretedChunksDecoder4Analyzer.hpp"
#include "deserialization/morphInterps/InterpretedChunksDecoder4Generator.hpp"

namespace morfeusz {

static Deserializer<InterpsGroupsReader>& initializeDeserializer(MorfeuszProcessorType processorType) {
    static Deserializer<InterpsGroupsReader> *analyzerDeserializer
            = new MorphDeserializer();
    static Deserializer<InterpsGroupsReader> *generatorDeserializer
            = new MorphDeserializer();
    return *(processorType == ANALYZER ? analyzerDeserializer : generatorDeserializer);
}

static void deleteSegrulesFSAs(std::map<SegrulesOptions, SegrulesFSA*>& fsasMap) {
    for (
            std::map<SegrulesOptions, SegrulesFSA*>::iterator it = fsasMap.begin();
            it != fsasMap.end();
            ++it) {
        delete it->second;
    }
    fsasMap.clear();
}

Environment::Environment(
        Charset charset,
        MorfeuszProcessorType processorType,
        const unsigned char* fsaFileStartPtr)
: currentCharsetConverter(getCharsetConverter(charset)),
caseConverter(),
tagset(fsaFileStartPtr),
qualifiers(fsaFileStartPtr),
fsaFileStartPtr(fsaFileStartPtr),
fsa(FSAType::getFSA(fsaFileStartPtr, initializeDeserializer(processorType))),
separatorsList(getSeparatorsList(fsaFileStartPtr)),
segrulesFSAsMap(createSegrulesFSAsMap(fsaFileStartPtr)),
currSegrulesOptions(getDefaultSegrulesOptions(fsaFileStartPtr)),
currSegrulesFSA(getDefaultSegrulesFSA(segrulesFSAsMap, fsaFileStartPtr)),
isFromFile(false),
chunksDecoder(
processorType == ANALYZER
? (InterpretedChunksDecoder*) new InterpretedChunksDecoder4Analyzer(*this)
: (InterpretedChunksDecoder*) new InterpretedChunksDecoder4Generator(*this)),
processorType(processorType),
casePatternHelper(new CasePatternHelper()) {
}

const CharsetConverter* Environment::getCharsetConverter(Charset charset) const {
    switch (charset) {
        case UTF8:
            return &UTF8CharsetConverter::getInstance();
        case ISO8859_2:
            return &ISO8859_2_CharsetConverter::getInstance();
        case CP1250:
            return &Windows_1250_CharsetConverter::getInstance();
        case CP852:
            return &CP852_CharsetConverter::getInstance();
        default:
            throw MorfeuszException("invalid charset");
    }
}

Environment::~Environment() {
    delete this->fsa;
    if (this->isFromFile) {
        deleteSegrulesFSAs(this->segrulesFSAsMap);
        delete this->fsaFileStartPtr;
    }
    delete this->chunksDecoder;
    delete this->casePatternHelper;
}

void Environment::setCharset(Charset charset) {
    this->currentCharsetConverter = this->getCharsetConverter(charset);
}

const CharsetConverter& Environment::getCharsetConverter() const {
    return *this->currentCharsetConverter;
}

const CaseConverter& Environment::getCaseConverter() const {
    return this->caseConverter;
}

void Environment::setTagset(const Tagset& tagset) {
    this->tagset = tagset;
}

const Tagset& Environment::getTagset() const {
    return this->tagset;
}

void Environment::setFSAFile(const std::string& filename) {
    if (this->isFromFile) {
        delete this->fsa;
        deleteSegrulesFSAs(this->segrulesFSAsMap);
        delete this->fsaFileStartPtr;
    }
    this->fsaFileStartPtr = readFile<unsigned char>(filename.c_str());
    this->fsa = FSA< InterpsGroupsReader > ::getFSA(fsaFileStartPtr, initializeDeserializer(this->processorType));
    this->separatorsList = getSeparatorsList(fsaFileStartPtr);
    this->segrulesFSAsMap = createSegrulesFSAsMap(this->fsaFileStartPtr);
    this->currSegrulesFSA = getDefaultSegrulesFSA(this->segrulesFSAsMap, this->fsaFileStartPtr);
    this->isFromFile = true;
    this->tagset = Tagset(fsaFileStartPtr);
    this->qualifiers = Qualifiers(fsaFileStartPtr);
}

const SegrulesFSA& Environment::getCurrentSegrulesFSA() const {
    return *(this->currSegrulesFSA);
}

const FSAType& Environment::getFSA() const {
    return *(this->fsa);
}

const InterpretedChunksDecoder& Environment::getInterpretedChunksDecoder() const {
    return *(this->chunksDecoder);
}

void Environment::setSegrulesOption(const std::string& option, const std::string& value) {
    if (this->currSegrulesOptions.find(option) == this->currSegrulesOptions.end()) {
        throw MorfeuszException("Invalid segmentation option '"+option+"'");
    }
    SegrulesOptions prevOptions = this->currSegrulesOptions;
    this->currSegrulesOptions[option] = value;
    if (this->segrulesFSAsMap.find(this->currSegrulesOptions) == this->segrulesFSAsMap.end()) {
        this->currSegrulesOptions = prevOptions;
        throw MorfeuszException("Invalid '"+option+"' option value: '"+value+"'");
    }
    this->currSegrulesFSA = this->segrulesFSAsMap.find(this->currSegrulesOptions)->second;
}

MorfeuszProcessorType Environment::getProcessorType() const {
    return this->processorType;
}

void Environment::setCaseSensitive(bool caseSensitive) {
    this->casePatternHelper->setCaseSensitive(caseSensitive);
}

const CasePatternHelper& Environment::getCasePatternHelper() const {
    return *this->casePatternHelper;
}

const Qualifiers& Environment::getQualifiersHelper() const {
    return this->qualifiers;
}

bool Environment::isSeparator(uint32_t codepoint) const {
    return binary_search(
            this->separatorsList.begin(), 
            this->separatorsList.end(), 
            codepoint);
}

}
