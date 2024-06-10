#pragma once
#include "token.h"
#include "operator.h"

enum
{
    TOKENIZER_FLAG_CEASE_ON_MISMATCH = 1 << 0,
};
typedef uint8_t TokenizerFlags;

extern RynError tokenize (size_t length, const char* source, TokenList* list, TokenizerFlags flags);

// EOF //
