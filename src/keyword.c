#include "keyword.h"

const char* const keywords[KEYWORD_COUNT] =
{
#define X(name,keyword) [KEYWORD_##name] = #keyword,
    KEYWORD_X_LIST
#undef X
};

