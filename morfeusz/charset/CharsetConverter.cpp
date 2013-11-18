
#include <vector>
#include <iterator>
#include "utf8.h"
#include "CharsetConverter.hpp"

using namespace std;

uint32_t UTF8CharsetConverter::next(const char*& it, const char* end) const {
    return utf8::next(it, end);
}

void UTF8CharsetConverter::append(uint32_t cp, vector<char>& result) const {
    utf8::append(cp, back_inserter(result));
}
