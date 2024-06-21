#include "keyword.h"

const char* const keywords[KEYWORD_COUNT] =
{
#define X(name,keyword,hasBlock) \
    [KEYWORD_##name] = #keyword,
    KEYWORD_X_LIST
#undef X
};

const bool keywordHasBlock[KEYWORD_COUNT] =
{
#define X(name,keyword,hasBlock) \
    [KEYWORD_##name] = hasBlock,
    KEYWORD_X_LIST
#undef X
};

