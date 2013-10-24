/* 
 * File:   fsa.hh
 * Author: mlenart
 *
 * Created on October 17, 2013, 2:00 PM
 */

#ifndef FSA_HPP
#define FSA_HPP

//#include <iostream>
#include <cstring>
#include <typeinfo>
#include <cassert>

template <class T> class State;
template <class T> class FSA;
template <class T> class Deserializer;

template <class T>
class Deserializer {
public:

    /**
     * Deserialize object from ptr.
     * Returns number of bytes read or -1 on error.
     */
    virtual int deserialize(const unsigned char* ptr, T& object) const = 0;
};

class StringDeserializer : public Deserializer<char*> {
public:

    StringDeserializer() {
    }

    /**
     * Deserialize object from ptr.
     * Returns number of bytes read or -1 on error.
     */
    int deserialize(const unsigned char* ptr, char*& text) const {
        text = const_cast<char*> (reinterpret_cast<const char*> (ptr));
        return strlen(text) + 1;
    }
};

/**
 * Finite state automaton.
 */
template <class T>
class FSA {
public:

    /**
     * Get this automaton's initial state.
     */
    State<T> getInitialState() const {
        return State<T>(*this);
    }
    
    bool tryToRecognize(const char* input, T& value) const {
        State<T> currState = this->getInitialState();
        int i = 0;
        while (!currState.isSink() && input[i] != '\0') {
            currState.proceedToNext(input[i]);
            i++;
        }
        if (currState.isAccepting()) {
            value = currState.getValue();
            return true;
        }
        else {
            return false;
        }
    }

    virtual ~FSA() {
    }
protected:

    FSA(const unsigned char* ptr, const Deserializer<T>& deserializer)
    : startPtr(ptr), deserializer(deserializer) {
    }
    /**
     * Proceed to next state
     * 
     * @param fromPointer - wskaźnik
     * @param c - char for the transition.
     * @return next state
     */
    virtual void proceedToNext(const char c, State<T>& state) const = 0;
    const unsigned char* startPtr;
    const Deserializer<T>& deserializer;
    friend class State<T>;
private:
    //    FSA();
};

template <class T>
class SimpleFSA : public FSA<T> {
public:
    SimpleFSA(const unsigned char* ptr, const Deserializer<T>& deserializer);
    virtual ~SimpleFSA();
protected:
    void proceedToNext(const char c, State<T>& state) const;
private:

};

#include "_simple_fsa_impl.hpp"

/**
 * A state in an FSA.
 */
template <class T>
class State {
public:

    /**
     * Is this a "sink" state - non-accepting state without outgoing transitions
     */
    bool isSink() const;

    /**
     * Is this an accepting state
     */
    bool isAccepting() const;

    /**
     * Get next state proceeding a transition for given character.
     */
    void proceedToNext(const char c);

    /**
     * Get value of this state.
     * Makes sense only for accepting states.
     * For non-accepting states is throws an exception.
     */
    T getValue() const;

    /**
     * Get the size (in bytes) of this state's value.
     * Makes sense only for accepting states.
     * For non-accepting states is throws an exception.
     */
    unsigned int getValueSize() const;
    
    unsigned int getOffset() const;
    
    void setNext(const unsigned int offset);
    void setNext(const unsigned int offset, const T& value, const unsigned int valueSize);
    void setNextAsSink();
    
    State(const FSA<T>& fsa);
    
    virtual ~State();
private:
    const FSA<T>& fsa;
    unsigned int offset;
    bool accepting;
    bool sink;
    T value;
    int valueSize;
};

#include "_state_impl.hpp"

#endif	/* FSA_HPP */


