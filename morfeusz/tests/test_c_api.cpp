/* 
 * File:   test_c_api.cpp
 * Author: lennyn
 *
 * Created on June 25, 2014, 7:43 PM
 */

#include <cstdlib>
#include <string>
#include <iostream>
#include "morfeusz2_c.h"
#include "validate.hpp"

using namespace std;

void testTwoSimpleInvocations() {
    char* text = const_cast<char*>(string("AAaaBBbbCCcc DDDD.").c_str());
    InterpMorf* results = morfeusz_analyse(text);
    validateEquals(0, results[0].p);
    validateEquals(1, results[0].k);
    validateEquals(string("AAaaBBbbCCcc"), string(results[0].forma));
    validateEquals(string("AAaaBBbbCCcc"), string(results[0].haslo));
    validateEquals(string("ign"), string(results[0].interp));
    
    validateEquals(1, results[1].p);
    validateEquals(2, results[1].k);
    validateEquals(string("DDDD"), string(results[1].forma));
    validateEquals(string("DDDD"), string(results[1].haslo));
    validateEquals(string("ign"), string(results[1].interp));
    
    validateEquals(2, results[2].p);
    validateEquals(3, results[2].k);
    validateEquals(string("."), string(results[2].forma));
    validateEquals(string("."), string(results[2].haslo));
//    validateEquals(string("ign"), string(results[2].interp));
    validateEquals(-1, results[3].p);
    
    char* text1 = const_cast<char*>(string("EEeeFFff").c_str());
    results = morfeusz_analyse(text1);
    validateEquals(0, results[0].p);
    validateEquals(1, results[0].k);
    validateEquals(string("EEeeFFff"), string(results[0].forma));
    validateEquals(string("EEeeFFff"), string(results[0].haslo));
    validateEquals(string("ign"), string(results[0].interp));
    
    validateEquals(-1, results[1].p);
}

void testWhitespaceKEEP() {
    
    validateEquals(1, morfeusz_set_option(MORFOPT_WHITESPACE, MORFEUSZ_KEEP_WHITESPACE));
    
    char* text = const_cast<char*>(string("AAaaBBbbCCcc  .").c_str());
    InterpMorf* results = morfeusz_analyse(text);
    validateEquals(0, results[0].p);
    validateEquals(1, results[0].k);
    validateEquals(string("AAaaBBbbCCcc"), string(results[0].forma));
    validateEquals(string("AAaaBBbbCCcc"), string(results[0].haslo));
    validateEquals(string("ign"), string(results[0].interp));
    
    validateEquals(1, results[1].p);
    validateEquals(2, results[1].k);
    validateEquals(string("  "), string(results[1].forma));
    validateEquals(string("  "), string(results[1].haslo));
//    validateEquals(string("ign"), string(results[2].interp));
    
    validateEquals(2, results[2].p);
    validateEquals(3, results[2].k);
    validateEquals(string("."), string(results[2].forma));
    validateEquals(string("."), string(results[2].haslo));
//    validateEquals(string("ign"), string(results[2].interp));
    
    validateEquals(-1, results[3].p);
}

void testWhitespaceAPPEND() {
    validateEquals(1, morfeusz_set_option(MORFOPT_WHITESPACE, MORFEUSZ_APPEND_WHITESPACE));
    
    char* text = const_cast<char*>(string("AAaaBBbbCCcc  .").c_str());
    InterpMorf* results = morfeusz_analyse(text);
    validateEquals(0, results[0].p);
    validateEquals(1, results[0].k);
    validateEquals(string("AAaaBBbbCCcc  "), string(results[0].forma));
    validateEquals(string("AAaaBBbbCCcc  "), string(results[0].haslo));
    validateEquals(string("ign"), string(results[0].interp));
    
    validateEquals(1, results[1].p);
    validateEquals(2, results[1].k);
    validateEquals(string("."), string(results[1].forma));
    validateEquals(string("."), string(results[1].haslo));
//    validateEquals(string("ign"), string(results[2].interp));
    
    validateEquals(-1, results[2].p);
}

void testEncodingUTF8() {
    unsigned char text[] = {'z', 'a', /* ż */ 197, 188, /* ó */ 195, 179, '\0'};
    char* actualText = (char*) text;
    validateEquals(1, morfeusz_set_option(MORFOPT_ENCODING, MORFEUSZ_CP1250));
    InterpMorf* results = morfeusz_analyse(actualText);
    validateEquals(0, results[0].p);
    validateEquals(1, results[0].k);
    validateEquals(string(actualText), string(results[0].forma));
    validateEquals(string(actualText), string(results[0].haslo));
    validateEquals(string("ign"), string(results[0].interp));
    validateEquals(-1, results[2].p);
}

void testTokenNumberingCONTINUOUS() {
    
    morfeusz_set_option(MORFOPT_WHITESPACE, MORFEUSZ_SKIP_WHITESPACE);
    validateEquals(1, morfeusz_set_option(MORFOPT_TOKEN_NUMBERING, MORFEUSZ_CONTINUOUS_TOKEN_NUMBERING));
    
    char* text = const_cast<char*>(string("a b.").c_str());
    InterpMorf* results = morfeusz_analyse(text);
    validateEquals(0, results[0].p);
    validateEquals(1, results[0].k);
    validateEquals(string("a"), string(results[0].forma));
    
    validateEquals(1, results[1].p);
    validateEquals(2, results[1].k);
    validateEquals(string("b"), string(results[1].forma));
    
    validateEquals(2, results[2].p);
    validateEquals(3, results[2].k);
    validateEquals(string("."), string(results[2].forma));
    
    validateEquals(-1, results[3].p);
    
    char* text1 = const_cast<char*>(string("c").c_str());
    results = morfeusz_analyse(text1);
    validateEquals(3, results[0].p);
    validateEquals(4, results[0].k);
    validateEquals(string("c"), string(results[0].forma));
    
    validateEquals(-1, results[1].p);
}

void testEncodingISO8859_2() {
    validateEquals(1, morfeusz_set_option(MORFOPT_ENCODING, MORFEUSZ_ISO8859_2));
}

void testEncodingCP1250() {
    validateEquals(1, morfeusz_set_option(MORFOPT_ENCODING, MORFEUSZ_CP1250));
}

void testEncodingCP852() {
    validateEquals(1, morfeusz_set_option(MORFOPT_ENCODING, MORFEUSZ_CP852));
}

void testWrongWhitespaceOption() {
    validateEquals(0, morfeusz_set_option(MORFOPT_WHITESPACE, 666777));
}

void testWrongEncodingOption() {
    validateEquals(0, morfeusz_set_option(MORFOPT_ENCODING, 666777));
}

void testWrongCaseOption() {
    validateEquals(0, morfeusz_set_option(MORFOPT_CASE, 666777));
}

void testWrongTokenNumberingOption() {
    validateEquals(0, morfeusz_set_option(MORFOPT_TOKEN_NUMBERING, 666777));
}

int main() {
    testTwoSimpleInvocations();
    testWhitespaceKEEP();
    testWhitespaceAPPEND();
    testEncodingUTF8();
    testEncodingISO8859_2();
    testEncodingCP1250();
    testEncodingCP852();
    testWrongWhitespaceOption();
    testWrongEncodingOption();
    testWrongCaseOption();
    testWrongTokenNumberingOption();
    testTokenNumberingCONTINUOUS();
    return 0;
}

