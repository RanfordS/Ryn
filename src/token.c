#include "token.h"

const char* const tokenType[TOKEN_TYPE_COUNT] =
{
#define X(name) [TOKEN_TYPE_##name] = #name,
    TOKEN_TYPE_X_LIST
#undef X
};



LISTIFY_C(Token)

void tokenListPrint (const char* source, const TokenList* tokenList)
{
    for (Size t = 0; t < tokenList->count; ++t)
    {
        Token* token = &tokenList->data[t];
        printf ("0x%04llX[%03llu,%02llu] - %s `%.*s` ID=%u\n",
                t, token->line, token->column,
                tokenType[token->type],
                token->length, source + token->index,
                token->ident);
        printf ("- index: l=0x%04llX, r=0x%04llX\n",
                token->leftIndex, token->rightIndex);
        //printf ("- flags 0x%02X\n", token->tokenFlags);
    }
}
