#include "token.h"

const char* const tokenType[TOKEN_TYPE_COUNT] =
{
#define X(name) [TOKEN_TYPE_##name] = #name,
    TOKEN_TYPE_X_LIST
#undef X
};



TokenList tokenListCreate (size_t size)
{
    TokenList new =
    {
        .size = size,
        .count = 0,
        .list = malloc (size*sizeof (*new.list)),
    };
    return new;
}

void tokenListAppend (TokenList* tokenList, Token token)
{
    size_t pos = tokenList->count++;
    if (tokenList->size <= tokenList->count)
    {
        size_t oldSize = tokenList->size;
        size_t newSize = tokenList->size *= 2;
        tokenList->list = realloc (tokenList->list, newSize);
        while (oldSize < newSize)
        {
            tokenList->list[oldSize++] = (Token){};
        }
    }
    tokenList->list[pos] = token;
}
