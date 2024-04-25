#pragma once
#include "token.h"
#include "operator.h"

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
    size_t bracketStackSize;
    Operator* bracketStackType;
    size_t* bracketIndex;
    bool escape;
}
Tokenizer;

extern bool tokenize (Tokenizer* tokenizer, TokenList* list);
extern Tokenizer tokenizerCreate (char* path, size_t length, char* data);
