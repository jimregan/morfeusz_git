/* 
 * File:   exceptions.hpp
 * Author: mlenart
 *
 * Created on 22 styczeń 2014, 13:16
 */

#ifndef EXCEPTIONS_HPP
#define	EXCEPTIONS_HPP

class MorfeuszException : public std::exception {
public:

    MorfeuszException(const std::string& what) : msg(what.c_str()) {
    }

    virtual ~MorfeuszException() throw () {
    }

    virtual const char* what() const throw () {
        return this->msg.c_str();
    }
private:
    const std::string msg;
};

#endif	/* EXCEPTIONS_HPP */

