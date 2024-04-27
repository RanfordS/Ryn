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
    TOKENIZER_ERROR_INVALID_SPECIFIER,
    TOKENIZER_ERROR_ZERO_FOLLOWED_BY_NONZERO,
    TOKENIZER_ERROR_INVALID_CHARACTER_AFTER_DECIMAL_POINT,
    TOKENIZER_ERROR_EXPONENT_MISSING,
    TOKENIZER_ERROR_SPECIFIER_SIZE_STARTS_WITH_ZERO,
    TOKENIZER_ERROR_UNEXPECTED_CHARACTER_IN_SIZE_SPECIFIER,
    TOKENIZER_ERROR_INVALID_NUMBER_IN_BINARY,
    TOKENIZER_ERROR_INVALID_NUMBER_IN_OCTAL,
    TOKENIZER_ERROR_INVALID_NUMBER_IN_HEXADECIMAL,
};
typedef uint8_t TokenizerCode;

extern TokenizerCode tokenize (Tokenizer* tokenizer, TokenList* list);
extern Tokenizer tokenizerCreate (char* path, size_t length, char* data);
