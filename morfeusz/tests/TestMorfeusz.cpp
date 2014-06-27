/*
 * File:   TestMorfeusz.cpp
 * Author: lennyn
 *
 * Created on Jun 27, 2014, 1:03:19 PM
 */

#include "TestMorfeusz.hpp"

#include <cstdio>
#include <vector>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION(TestMorfeusz);

using namespace std;
using namespace morfeusz;

TestMorfeusz::TestMorfeusz() {
}

TestMorfeusz::~TestMorfeusz() {
}

void TestMorfeusz::setUp() {
    morfeusz = Morfeusz::createInstance();
}

void TestMorfeusz::tearDown() {
    delete morfeusz;
}

void TestMorfeusz::testAnalyzeIterate1() {
    ResultsIterator* it = morfeusz->analyze("AAAAbbbbCCCC");
    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string("AAAAbbbbCCCC"), it->peek().getOrth());
    CPPUNIT_ASSERT_EQUAL(string("AAAAbbbbCCCC"), it->next().getOrth());
    CPPUNIT_ASSERT(!it->hasNext());
    CPPUNIT_ASSERT_THROW(it->peek(), MorfeuszException);
    CPPUNIT_ASSERT_THROW(it->next(), MorfeuszException);
    delete it;
}

void TestMorfeusz::testAnalyzeVector1() {
    vector<MorphInterpretation> res;
    morfeusz->analyze("AAAAbbbbCCCC", res);
    CPPUNIT_ASSERT_EQUAL((size_t) 1, res.size());
    CPPUNIT_ASSERT_EQUAL(string("AAAAbbbbCCCC"), res[0].getOrth());
    CPPUNIT_ASSERT_EQUAL(string("AAAAbbbbCCCC"), res[0].getLemma());
}

static inline string prepareErrorneusTmpFile() {
    char* filename = tmpnam(NULL);
    ofstream out;
    out.open(filename);
    out << "asfasdfa" << endl;
    out.close();
    return string(filename);
}

void TestMorfeusz::testOpenInvalidFile() {
    string filename(prepareErrorneusTmpFile());
    CPPUNIT_ASSERT_THROW(morfeusz->setAnalyzerDictionary(filename), FileFormatException);
}

void TestMorfeusz::testOpenNonExistentFile() {
    string filename(tmpnam(NULL));
    CPPUNIT_ASSERT_THROW(morfeusz->setAnalyzerDictionary(filename), std::ios_base::failure);
}

void TestMorfeusz::testSetInvalidAgglOption() {
    CPPUNIT_ASSERT_THROW(morfeusz->setAggl("asdfasdfa"), MorfeuszException);
}

void TestMorfeusz::testSetInvalidPraetOption() {
    CPPUNIT_ASSERT_THROW(morfeusz->setPraet("asdfasdfa"), MorfeuszException);
}

void TestMorfeusz::testWhitespaceHandlingKEEP() {
    
}

void TestMorfeusz::testWhitespaceHandlingAPPEND() {

}
