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
    cerr << "testAnalyzeIterate1" << endl;
    ResultsIterator* it = morfeusz->analyze("AAAAbbbbCCCC");
    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string("AAAAbbbbCCCC"), it->peek().getOrth());
    CPPUNIT_ASSERT_EQUAL(string("AAAAbbbbCCCC"), it->next().getOrth());
    CPPUNIT_ASSERT(!it->hasNext());
    CPPUNIT_ASSERT_THROW(it->peek(), MorfeuszException);
    CPPUNIT_ASSERT_THROW(it->next(), MorfeuszException);
    delete it;
}

void TestMorfeusz::testAnalyzeIterateWithWhitespaceHandlingKEEP() {
    cerr << "testAnalyzeIterateWithWhitespaceHandlingKEEP" << endl;
    morfeusz->setWhitespaceHandling(KEEP_WHITESPACES);
    ResultsIterator* it = morfeusz->analyze(" AAAAbbbbCCCC  DDDDeeee.\t");
    
    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string(" "), it->next().getOrth());
    
    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string("AAAAbbbbCCCC"), it->next().getOrth());
    
    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string("  "), it->next().getOrth());
    
    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string("DDDDeeee"), it->next().getOrth());
    
    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string("."), it->next().getOrth());
    
    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string("\t"), it->next().getOrth());
    
    CPPUNIT_ASSERT(!it->hasNext());
    CPPUNIT_ASSERT_THROW(it->peek(), MorfeuszException);
    CPPUNIT_ASSERT_THROW(it->next(), MorfeuszException);
    delete it;
}

void TestMorfeusz::testAnalyzeIterateWithWhitespaceHandlingAPPEND() {
    cerr << "testAnalyzeIterateWithWhitespaceHandlingAPPEND" << endl;
    morfeusz->setWhitespaceHandling(APPEND_WHITESPACES);
    ResultsIterator* it = morfeusz->analyze(" AAAAbbbbCCCC  DDDDeeee.\t");
    
    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string(" AAAAbbbbCCCC  "), it->next().getOrth());
    
    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string("DDDDeeee"), it->next().getOrth());
    
    CPPUNIT_ASSERT(it->hasNext());
    CPPUNIT_ASSERT_EQUAL(string(".\t"), it->next().getOrth());
    
    CPPUNIT_ASSERT(!it->hasNext());
    CPPUNIT_ASSERT_THROW(it->peek(), MorfeuszException);
    CPPUNIT_ASSERT_THROW(it->next(), MorfeuszException);
    delete it;
}

void TestMorfeusz::testAnalyzeVector1() {
    cerr << "testAnalyzeVector1" << endl;
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
    cerr << "testOpenInvalidFile" << endl;
    string filename(prepareErrorneusTmpFile());
    CPPUNIT_ASSERT_THROW(morfeusz->setAnalyzerDictionary(filename), FileFormatException);
}

void TestMorfeusz::testOpenNonExistentFile() {
    cerr << "testOpenNonExistentFile" << endl;
    string filename(tmpnam(NULL));
    CPPUNIT_ASSERT_THROW(morfeusz->setAnalyzerDictionary(filename), std::ios_base::failure);
}

void TestMorfeusz::testSetInvalidAgglOption() {
    cerr << "testSetInvalidAgglOption" << endl;
    CPPUNIT_ASSERT_THROW(morfeusz->setAggl("asdfasdfa"), MorfeuszException);
}

void TestMorfeusz::testSetInvalidPraetOption() {
    cerr << "testSetInvalidPraetOption" << endl;
    CPPUNIT_ASSERT_THROW(morfeusz->setPraet("asdfasdfa"), MorfeuszException);
}

void TestMorfeusz::testWhitespaceHandlingKEEP() {
    cerr << "testWhitespaceHandlingKEEP" << endl;
    vector<MorphInterpretation> res;
    morfeusz->setWhitespaceHandling(KEEP_WHITESPACES);
    morfeusz->analyze("  AAAAbbbbCCCC DDDDeeee\t", res);
    CPPUNIT_ASSERT_EQUAL((size_t) 5, res.size());
    CPPUNIT_ASSERT_EQUAL(string("  "), res[0].getOrth());
    CPPUNIT_ASSERT_EQUAL(string("  "), res[0].getLemma());
    CPPUNIT_ASSERT_EQUAL(1, res[0].getTagnum());
    CPPUNIT_ASSERT_EQUAL(string("AAAAbbbbCCCC"), res[1].getOrth());
    CPPUNIT_ASSERT_EQUAL(string("AAAAbbbbCCCC"), res[1].getLemma());
    CPPUNIT_ASSERT_EQUAL(0, res[1].getTagnum());
    CPPUNIT_ASSERT_EQUAL(string(" "), res[2].getOrth());
    CPPUNIT_ASSERT_EQUAL(string(" "), res[2].getLemma());
    CPPUNIT_ASSERT_EQUAL(1, res[2].getTagnum());
    CPPUNIT_ASSERT_EQUAL(string("DDDDeeee"), res[3].getOrth());
    CPPUNIT_ASSERT_EQUAL(string("DDDDeeee"), res[3].getLemma());
    CPPUNIT_ASSERT_EQUAL(0, res[3].getTagnum());
    CPPUNIT_ASSERT_EQUAL(string("\t"), res[4].getOrth());
    CPPUNIT_ASSERT_EQUAL(string("\t"), res[4].getLemma());
    CPPUNIT_ASSERT_EQUAL(1, res[4].getTagnum());
}

void TestMorfeusz::testWhitespaceHandlingAPPEND() {
    cerr << "testWhitespaceHandlingAPPEND" << endl;
    vector<MorphInterpretation> res;
    morfeusz->setWhitespaceHandling(APPEND_WHITESPACES);
    morfeusz->analyze("  AAAAbbbbCCCC DDDDeeee\t", res);
    CPPUNIT_ASSERT_EQUAL((size_t) 2, res.size());
    CPPUNIT_ASSERT_EQUAL(string("  AAAAbbbbCCCC "), res[0].getOrth());
    CPPUNIT_ASSERT_EQUAL(string("AAAAbbbbCCCC"), res[0].getLemma());
    CPPUNIT_ASSERT_EQUAL(0, res[0].getTagnum());
    CPPUNIT_ASSERT_EQUAL(string("DDDDeeee\t"), res[1].getOrth());
    CPPUNIT_ASSERT_EQUAL(string("DDDDeeee"), res[1].getLemma());
    CPPUNIT_ASSERT_EQUAL(0, res[1].getTagnum());
}
