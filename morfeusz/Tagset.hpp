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

class Tagset {
public:
    explicit Tagset(const unsigned char* fsaData);
    const std::string& getTag(const int tagNum) const;
    const std::string& getName(const int nameNum) const;
private:
    std::vector<std::string> tags;
    std::vector<std::string> names;
};

#endif	/* TAGSET_HPP */
