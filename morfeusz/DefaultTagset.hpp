/* 
 * File:   tagset.hpp
 * Author: mlenart
 *
 * Created on 12 listopad 2013, 14:09
 */

#ifndef DEFAULTTAGSET_HPP
#define	DEFAULTTAGSET_HPP

#include <string>
#include <vector>
#include "morfeusz2.h"

namespace morfeusz {

    /**
     * Represents a tagset
     */
    class DefaultTagset : public Tagset<std::string> {
    public:
        /**
         * Constructs a tagset from binary data.
         * 
         * @param fsaData - pointer to the beginning of automaton data.
         */
        explicit DefaultTagset(const unsigned char* fsaData);

        const std::string& getTag(const int tagNum) const;

        const std::string& getName(const int nameNum) const;

        size_t getTagsSize() const;

        size_t getNamesSize() const;
    private:
        std::vector<std::string> tags;
        std::vector<std::string> names;
    };

}

#endif	/* DEFAULTTAGSET_HPP */

