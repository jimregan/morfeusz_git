
#include <string>
#include <vector>
#include "DefaultTagset.hpp"
#include "fsa/const.hpp"
#include "utils.hpp"
#include "deserialization/deserializationUtils.hpp"

using namespace std;

namespace morfeusz {

    DefaultTagset::DefaultTagset(const unsigned char* ptr, const CharsetConverter* charsetConverter)
    : tags(),
    names(),
    charsetConverter(charsetConverter) {
        uint32_t fsaSize = readInt32Const(ptr + FSA_DATA_SIZE_OFFSET);
        const unsigned char* currPtr = ptr + FSA_DATA_OFFSET + fsaSize + 4;
        readTags(currPtr, this->tags);
        readTags(currPtr, this->names);
        setCharsetConverter(charsetConverter);
    }

    const string& DefaultTagset::getTag(const int tagNum) const {
        return this->tags.at(tagNum);
//        if (charsetConverter == &UTF8CharsetConverter::getInstance()) {
//            return this->tags.at(tagNum);
//        }
//        else {
//            return charsetConverter->fromUTF8(this->tags.at(tagNum));
//        }
    }

    const string& DefaultTagset::getName(const int nameNum) const {
        return this->names.at(nameNum);
//        if (charsetConverter == &UTF8CharsetConverter::getInstance()) {
//            return this->names.at(nameNum);
//        }
//        else {
//            return charsetConverter->fromUTF8(this->names.at(nameNum));
//        }
    }

    size_t DefaultTagset::getTagsSize() const {
        return this->tags.size();
    }

    size_t DefaultTagset::getNamesSize() const {
        return this->names.size();
    }

    // FIXME - probably should not convert whole tagset on every setCharsetConverter method invocation.
    void DefaultTagset::setCharsetConverter(const CharsetConverter* charsetConverter) {
        
        for (unsigned int i = 0; i < tags.size(); i++) {
            tags[i] = charsetConverter->fromUTF8(
                    this->charsetConverter->toUTF8(tags[i]));
        }
        
        for (unsigned int j = 0; j < names.size(); j++) {
            names[j] = charsetConverter->fromUTF8(
                    this->charsetConverter->toUTF8(names[j]));
        }
        
        this->charsetConverter = charsetConverter;
    }

}
