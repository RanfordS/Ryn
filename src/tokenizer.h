#pragma once
#include "token.h"
#include "operator.h"

enum
{
    TOKENIZER_FLAG_CEASE_ON_MISMATCH = 1 << 0,
};
typedef U8 TokenizerFlags;

extern RynError tokenize (Size length, const char* source, TokenList* list, TokenizerFlags flags);

// EOF //
