/* 
 * File:   SegrulesDeserializer.hpp
 * Author: mlenart
 *
 * Created on 25 luty 2014, 16:16
 */

#ifndef SEGRULESDESERIALIZER_HPP
#define	SEGRULESDESERIALIZER_HPP

#include "../fsa/fsa.hpp"

class SegrulesDeserializer: public Deserializer<unsigned char> {
public:
    SegrulesDeserializer();
    long deserialize(const unsigned char* ptr, unsigned char& object) const;
    virtual ~SegrulesDeserializer();
private:

};

#endif	/* SEGRULESDESERIALIZER_HPP */

