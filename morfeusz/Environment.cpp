/* 
 * File:   Environment.cpp
 * Author: mlenart
 * 
 * Created on 22 styczeń 2014, 12:08
 */

#include "Environment.hpp"
#include "InterpretedChunksDecoder.hpp"
#include "MorphDeserializer.hpp"
#include "exceptions.hpp"

//class InterpretedChunksDecoder4Analyzer;
//class InterpretedChunksDecoder4Generator;

static Deserializer<vector<InterpsGroup> >* initializeDeserializer() {
    static Deserializer < vector < InterpsGroup > > *deserializer
            = new MorphDeserializer();
    return deserializer;
}

static SegrulesFSA* getDefaultSegrulesFSA(const map<SegrulesOptions, SegrulesFSA*>& map) {
    SegrulesOptions opts;
    opts["aggl"] = "isolated";
    opts["praet"] = "split";
    return (*(map.find(opts))).second;
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
        MorfeuszCharset charset,
        MorfeuszProcessorType processorType,
        const unsigned char* fsaFileStartPtr)
: currentCharsetConverter(getCharsetConverter(charset)),
        utf8CharsetConverter(),
        isoCharsetConverter(),
        cp1250CharsetConverter(),
        cp852CharsetConverter(),
        caseConverter(),
        tagset(fsaFileStartPtr),
        fsaFileStartPtr(fsaFileStartPtr),
        fsa(FSAType::getFSA(fsaFileStartPtr, *initializeDeserializer())),
        segrulesFSAsMap(createSegrulesFSAsMap(fsaFileStartPtr)),
        currSegrulesFSA(getDefaultSegrulesFSA(segrulesFSAsMap)),
        isFromFile(false),
        chunksDecoder(
            processorType == ANALYZER
            ? (InterpretedChunksDecoder*) new InterpretedChunksDecoder4Analyzer(*this)
            : (InterpretedChunksDecoder*) new InterpretedChunksDecoder4Generator(*this))
         {
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
    delete this->fsa;
    if (this->isFromFile) {
        deleteSegrulesFSAs(this->segrulesFSAsMap);
        delete this->fsaFileStartPtr;
    }
    delete this->chunksDecoder;
}

void Environment::setCharset(MorfeuszCharset charset) {
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
    this->fsa = FSA< vector<InterpsGroup> > ::getFSA(fsaFileStartPtr, *initializeDeserializer());
    this->segrulesFSAsMap = createSegrulesFSAsMap(this->fsaFileStartPtr);
    this->isFromFile = true;
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
