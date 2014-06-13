/* 
 * File:   tagset.hpp
 * Author: mlenart
 *
 * Created on 12 listopad 2013, 14:09
 */

#ifndef TAGSET_HPP
#define	TAGSET_HPP

#include <string>
#include <vector>
#include "charset/CharsetConverter.hpp"

namespace morfeusz {

/**
 * Represents a tagset
 */
class Tagset {
public:
    /**
     * Constructs a tagset from binary data.
     * 
     * @param fsaData - pointer to the beginning of automaton data.
     */
    explicit Tagset(const unsigned char* fsaData);
    
    /**
     * Returns tag (denoted by its index) as a string.
     * 
     * @param tagNum - tag index in the tagset.
     * @param charsetConverter - the charset converter used to convert from tagset internal encoding (UTF-8) into target encoding.
     * @return - the tag encoded as string.
     */
    const std::string getTag(const int tagNum, const CharsetConverter& charsetConverter) const;
    
    /**
     * Returns named entity type (denoted by its index) as a string.
     * 
     * @param nameNum - name index in the tagset.
     * @param charsetConverter - the charset converter used to convert from tagset internal encoding (UTF-8) into target encoding.
     * @return - the named entity type encoded as string.
     */
    const std::string getName(const int nameNum, const CharsetConverter& charsetConverter) const;
private:
    std::vector<std::string> tags;
    std::vector<std::string> names;
};

}

#endif	/* TAGSET_HPP */

