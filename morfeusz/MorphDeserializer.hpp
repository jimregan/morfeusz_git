/* 
 * File:   MorphDeserializer.hpp
 * Author: mlenart
 *
 * Created on 12 listopad 2013, 15:31
 */

#ifndef MORPHDESERIALIZER_HPP
#define	MORPHDESERIALIZER_HPP

#include <vector>
#include "fsa.hpp"
#include "interpretations.hpp"

class MorphDeserializer: public Deserializer<std::vector<Interpretation>> {
public:
    MorphDeserializer();
    MorphDeserializer(const MorphDeserializer& orig);
    virtual ~MorphDeserializer();
    long deserialize(
        const unsigned char* ptr, 
        std::vector<Interpretation>& interps) const;
private:

};

#endif	/* MORPHDESERIALIZER_HPP */

