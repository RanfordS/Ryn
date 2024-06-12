#pragma once
#include "core.h"

extern bool nstringsMatch (Size aLen, const char* aStr, Size bLen, const char* bStr);
extern bool nstringMatchCstring (Size nLen, const char* const nStr, const char* const cStr);
extern char* readFile (const char* path, Size* dataLength);

