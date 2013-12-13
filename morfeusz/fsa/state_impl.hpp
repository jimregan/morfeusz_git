/* 
 * File:   _state_impl.hpp
 * Author: mlenart
 *
 * Created on 21 październik 2013, 15:20
 */

#ifndef _STATE_IMPL_HPP
#define	_STATE_IMPL_HPP

#include <typeinfo>
#include <cstdio>
#include "fsa.hpp"

using namespace std;

template <class T>
State<T>::State(const FSA<T>& fsa)
: fsa(fsa), offset(0), accepting(false), sink(false), value(), valueSize(0) {
}

template <class T>
bool State<T>::isSink() const {
    return this->sink;
}

template <class T>
bool State<T>::isAccepting() const {
    return this->accepting;
}

template <class T>
void State<T>::proceedToNext(const char c) {
    printf("%d %c\n", static_cast<unsigned char>(c), c);
//    std::cerr << "NEXT " << std::hex << std::setw(2) << std::setfill('0') << int(c) << std::endl;
    if (this->sink) {
        return;
    }
    else {
        this->fsa.proceedToNext(c, *this);
    }
}

template <class T>
unsigned long State<T>::getOffset() const {
    assert(!this->isSink());
    return this->offset;
}

template <class T>
T State<T>::getValue() const {
    assert(this->isAccepting());
    return this->value;
}

template <class T>
unsigned long State<T>::getValueSize() const {
    assert(this->isAccepting());
    return this->valueSize;
}

template <class T>
State<T>::~State() {

}

template <class T>
void State<T>::setNext(const unsigned long offset) {
//    assert(!this->isSink());
    this->offset = offset;
    this->accepting = false;
}

template <class T>
void State<T>::setNext(const unsigned long offset, const T& value, const unsigned long valueSize) {
//    assert(!this->isSink());
    this->offset = offset;
    this->accepting = true;
    this->value = value;
    this->valueSize = valueSize;
}

template <class T>
void State<T>::setNextAsSink() {
    this->sink = true;
    this->accepting = false;
}

#endif	/* _STATE_IMPL_HPP */

