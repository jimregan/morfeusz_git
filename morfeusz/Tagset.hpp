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

class Tagset {
public:
    explicit Tagset(const unsigned char* fsaData);
//    Tagset(const Tagset& tagset);
    const std::string getTag(const int tagNum, const CharsetConverter& charsetConverter) const;
    const std::string getName(const int nameNum, const CharsetConverter& charsetConverter) const;
private:
    std::vector<std::string> tags;
    std::vector<std::string> names;
};

#endif	/* TAGSET_HPP */

