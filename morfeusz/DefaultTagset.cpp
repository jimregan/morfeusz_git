
#include <string>
#include <vector>
#include "DefaultTagset.hpp"
#include "fsa/const.hpp"
#include "utils.hpp"
#include "deserialization/deserializationUtils.hpp"

using namespace std;

namespace morfeusz {

DefaultTagset::DefaultTagset(const unsigned char* ptr) {
    uint32_t fsaSize = readInt32Const(ptr + FSA_DATA_SIZE_OFFSET);
    const unsigned char* currPtr = ptr + FSA_DATA_OFFSET + fsaSize + 4;
    readTags(currPtr, this->tags);
    readTags(currPtr, this->names);
}

const string& DefaultTagset::getTag(const int tagNum) const {
    return this->tags.at(tagNum);
}

const string& DefaultTagset::getName(const int nameNum) const {
    return this->names.at(nameNum);
}

size_t DefaultTagset::getTagsSize() const {
    return this->tags.size();
}

size_t DefaultTagset::getNamesSize() const {
    return this->names.size();
}

}
