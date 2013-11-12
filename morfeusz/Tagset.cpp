
#include <string>
#include <netinet/in.h>
#include "Tagset.hpp"
#include "const.hpp"
#include "utils.hpp"

using namespace std;

static uint16_t readInt16(const unsigned char*& currPtr) {
    DEBUG("readInt16");
    uint16_t res = htons(*reinterpret_cast<const uint16_t*>(currPtr));
    DEBUG("still alive " + to_string(res));
    currPtr += 2;
    DEBUG("still alive after ptr add");
    return res;
}

static string readString(const unsigned char*& currPtr) {
    DEBUG("readString");
    string res(reinterpret_cast<const char*>(currPtr));
    currPtr += res.length();
    currPtr++;
    return res;
}

static void readTags(const unsigned char*& currPtr, vector<string>& tags) {
    tags.clear();
    tags.resize(65536);
    uint16_t tagsNum = readInt16(currPtr);
    DEBUG("hi there");
    DEBUG("tagsNum="+to_string((int) tagsNum));
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
    DEBUG("will read tags");
    readTags(currPtr, this->tags);
    DEBUG("will read names");
    readTags(currPtr, this->names);
}

const string& Tagset::getTag(const int tagNum) const {
    return this->tags.at(tagNum);
}

const string& Tagset::getName(const int nameNum) const {
    return this->names.at(nameNum);
}
