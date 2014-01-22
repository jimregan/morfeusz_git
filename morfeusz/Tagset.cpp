
#include <string>
#include "Tagset.hpp"
#include "fsa/const.hpp"
#include "utils.hpp"
#include "endianness.hpp"

using namespace std;

static uint16_t readInt16(const unsigned char*& currPtr) {
    uint16_t res = htons(*reinterpret_cast<const uint16_t*>(currPtr));
    currPtr += 2;
    return res;
}

static string readString(const unsigned char*& currPtr) {
    string res(reinterpret_cast<const char*>(currPtr));
    currPtr += res.length();
    currPtr++;
    return res;
}

static void readTags(const unsigned char*& currPtr, vector<string>& tags) {
    tags.clear();
    tags.resize(65536);
    uint16_t tagsNum = readInt16(currPtr);
    for (unsigned int i = 0; i < tagsNum; i++) {
        unsigned int tagNum = readInt16(currPtr);
        tags[tagNum] = readString(currPtr);
    }
}

Tagset::Tagset(const unsigned char* fsaData) {
    const unsigned char* currPtr = fsaData + ADDITIONAL_DATA_OFFSET;
    readTags(currPtr, this->tags);
    readTags(currPtr, this->names);
}

//Tagset::Tagset(const Tagset& tagset)
//: tags(tagset.tags), names(tagset.names) {
//    
//}

const string Tagset::getTag(const int tagNum, const CharsetConverter& charsetConverter) const {
    return charsetConverter.fromUTF8(this->tags.at(tagNum));
}

const string Tagset::getName(const int nameNum, const CharsetConverter& charsetConverter) const {
    return charsetConverter.fromUTF8(this->names.at(nameNum));
}
