#pragma once
#include "core.h"

extern bool nstringsMatch (size_t aLen, const char* aStr, size_t bLen, const char* bStr);
extern bool nstringMatchCstring (size_t nLen, const char* const nStr, const char* const cStr);
extern char* readFile (const char* path, size_t* dataLength);

