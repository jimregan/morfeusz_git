/* 
 * File:   fsa.hh
 * Author: mlenart
 *
 * Created on October 17, 2013, 2:00 PM
 */

#ifndef FSA_HPP
#define FSA_HPP

//#include <iostream>
//#include <cstring>
#include <typeinfo>
#include <cassert>
#include <exception>
#include <string>
#include <vector>

template <class T> class State;
template <class T> class FSA;
template <class T> class Deserializer;
class FSAException;

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
        //        text = const_cast<char*> (reinterpret_cast<const char*> (ptr));
        //        return strlen(text) + 1;
        return 1;
    }
};

class Counter {
public:

    Counter() : count(0) {

    }

    void increment(const int n) {
        count += n;
    }
    long long count;
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
    State<T> getInitialState() const;

    bool tryToRecognize(const char* input, T& value) const;

    virtual ~FSA() {
    }
protected:

    FSA(const unsigned char* ptr, const Deserializer<T>& deserializer)
    : startPtr(ptr), deserializer(deserializer) {
    }
    /**
     * Proceed to next state
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

    long long transitionsCount() {
        return counter.count;
    }
protected:
    void proceedToNext(const char c, State<T>& state) const;
private:
    Counter counter;
};

template <class T>
class FSAImpl : public FSA<T> {
public:
    FSAImpl(const unsigned char* ptr, const Deserializer<T>& deserializer);
    virtual ~FSAImpl();

    long long transitionsCount() {
        return counter.count;
    }

    static const uint32_t MAGIC_NUMBER = 0x8fc2bc1b;
    static const unsigned char VERSION_NUM = 4;
    
    static const unsigned char ACCEPTING_FLAG =         0b10000000;
    static const unsigned char ARRAY_FLAG =             0b01000000;
    static const unsigned char TRANSITIONS_NUM_MASK =   0b00111111;

protected:
    void proceedToNext(const char c, State<T>& state) const;
private:
    Counter counter;
    const std::vector<unsigned char> label2ShortLabel;

    static int getMagicNumberOffset();
    static int getVersionNumOffset();
    static int getPopularCharsOffset();
    static int getInitialStateOffset();
    static std::vector<unsigned char> initializeChar2PopularCharIdx(const unsigned char* ptr);
    void doProceedToNextByList(
        const char c,
        const unsigned char shortLabel,
        const unsigned char* ptr, 
        const unsigned int transitionsNum, 
        State<T>& state) const;
    void doProceedToNextByArray(
        const unsigned char shortLabel, 
        const uint32_t* ptr, 
        State<T>& state) const;
    void reallyDoProceed(
        const unsigned char* statePtr,
        State<T>& state) const;
};

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

class FSAException : public std::exception {
public:
    FSAException(const char* what): msg(what) {}
    virtual ~FSAException() throw() {}
    virtual const char* what() const throw () {
        return this->msg.c_str();
    }
private:
    const std::string msg;
};

#include "_fsa_impl.hpp"
#include "_fsaimpl.hpp"
//#include "_vfsa_impl.hpp"
#include "_state_impl.hpp"

#endif	/* FSA_HPP */


