
#include <string>
#include "Tagset.hpp"
#include "fsa/const.hpp"
#include "utils.hpp"
#include "endianness.hpp"
#include "deserializationUtils.hpp"

using namespace std;

Tagset::Tagset(const unsigned char* ptr) {
    uint32_t fsaSize = readInt32Const(ptr + FSA_DATA_SIZE_OFFSET);
    const unsigned char* currPtr = ptr + FSA_DATA_OFFSET + fsaSize + 4;
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
