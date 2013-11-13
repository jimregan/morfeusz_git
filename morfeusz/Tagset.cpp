
#include <string>
#include <netinet/in.h>
#include "Tagset.hpp"
#include "const.hpp"
#include "utils.hpp"

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
//    uint32_t tagsNum = ntohl(*reinterpret_cast<const uint32_t*>(currPtr));
//    uint32_t namesNum = ntohl(*reinterpret_cast<const uint32_t*>(fsaData + ADDITIONAL_DATA_OFFSET + 4));
//    const unsigned char* currPtr = fsaData + 8;
    readTags(currPtr, this->tags);
    readTags(currPtr, this->names);
}

const string& Tagset::getTag(const int tagNum) const {
    return this->tags.at(tagNum);
}

const string& Tagset::getName(const int nameNum) const {
    return this->names.at(nameNum);
}
