/* 
 * File:   Environment.cpp
 * Author: mlenart
 * 
 * Created on 22 styczeń 2014, 12:08
 */

#include "Environment.hpp"
#include "exceptions.hpp"

Environment::Environment(
        const Tagset& analyzerTagset,
        const Tagset& generatorTagset,
        MorfeuszCharset charset)
: currentCharsetConverter(getCharsetConverter(charset)),
        utf8CharsetConverter(),
        isoCharsetConverter(),
        cp1250CharsetConverter(),
        cp852CharsetConverter(),
        analyzerTagset(analyzerTagset),
        generatorTagset(generatorTagset),
        caseConverter() {

}

const CharsetConverter* Environment::getCharsetConverter(MorfeuszCharset charset) const {
    switch (charset) {
        case UTF8:
            return &this->utf8CharsetConverter;
        case ISO8859_2:
            return &this->isoCharsetConverter;
        case CP1250:
            return &this->cp1250CharsetConverter;
        case CP852:
            return &this->cp852CharsetConverter;
        default:
            throw MorfeuszException("invalid charset");
    }
}

Environment::~Environment() {
}

void Environment::setCharset(MorfeuszCharset charset) {
    this->currentCharsetConverter = this->getCharsetConverter(charset);
}

const CharsetConverter& Environment::getCharsetConverter() const {
    return *this->currentCharsetConverter;
}

void Environment::setAnalyzerTagset(const Tagset& tagset) {
    this->analyzerTagset = tagset;
}

const Tagset& Environment::getAnalyzerTagset() const {
    return this->analyzerTagset;
}

void Environment::setGeneratorTagset(const Tagset& tagset) {
    this->generatorTagset = tagset;
}

const Tagset& Environment::getGeneratorTagset() const {
    return this->generatorTagset;
}

const CaseConverter& Environment::getCaseConverter() const {
    return this->caseConverter;
}
