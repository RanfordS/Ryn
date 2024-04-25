#pragma once
#include "token.h"

typedef struct s_Tokenizer
{
    Token currentToken;
    char* sourcePath;
    size_t sourceLength;
    char* sourceData;
    size_t line;
    size_t col;
    size_t commentDepth;
    size_t bracketDepth;
    bool escape;
}
Tokenizer;

extern bool tokenize (Tokenizer* tokenizer, TokenList* list);
