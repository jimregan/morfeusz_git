
#include "const.hpp"

#ifdef _WIN64
extern const MorfeuszCharset DEFAULT_MORFEUSZ_CHARSET = UTF16_LE;
#else
#ifdef _WIN32
extern const MorfeuszCharset DEFAULT_MORFEUSZ_CHARSET = UTF16_LE;
#else
extern const MorfeuszCharset DEFAULT_MORFEUSZ_CHARSET = UTF8;
#endif
#endif
