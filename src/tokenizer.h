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

enum
{
    TOKENIZER_CONTINUE,
    TOKENIZER_FINISHED,
    TOKENIZER_END_TOKEN_NOW,
    TOKENIZER_END_TOKEN_AFTER,
    TOKENIZER_ERROR_MISMATCHED_BRACKET,
    TOKENIZER_ERROR_TOO_MANY_CLOSING_BRACKETS,
    TOKENIZER_ERROR_UNHANDLED_BRACKET_TYPE,
};
typedef uint8_t TokenizerCode;

extern TokenizerCode tokenize (Tokenizer* tokenizer, TokenList* list);
extern Tokenizer tokenizerCreate (char* path, size_t length, char* data);
