#pragma once
#include "core.h"

#define TOKEN_TYPE_X_LIST \
    X(UNKNOWN)\
    X(NOUN)\
        X(KEYWORD)\
        X(IDENTIFIER)\
            X(FUNCTION)\
            X(VARIABLE)\
    X(GENERIC_TYPE)\
        X(TYPE)\
            X(FUNC)\
            X(STRUCT)\
            X(ENUM)\
    X(OPERATOR)\
    X(LITERAL_STRING)\
    X(LITERAL_CHAR)\
    X(LITERAL_NUMBER)\
        X(LITERAL_INTEGER)\
            X(LITERAL_INTEGER_SPECIFIED)\
        X(LITERAL_UNSIGNED)\
            X(LITERAL_UNSIGNED_SPECIFIED)\
        X(LITERAL_FLOAT)\
            X(LITERAL_FLOAT_SPECIFIED)\
                X(LITERAL_EXPONENTIAL_FLOAT_SPECIFIED)\
        X(LITERAL_DECIMAL)\
            X(LITERAL_EXPONENTIAL)\
                X(LITERAL_EXPONENTIAL_FLOAT)\
                X(LITERAL_EXPONENTIAL_DOUBLE)\
    X(LITERAL_ZERO)\
        X(LITERAL_HEXADECIMAL)\
            X(LITERAL_HEXADECIMAL_INTEGER)\
                X(LITERAL_HEXADECIMAL_INTEGER_SPECIFIED)\
            X(LITERAL_HEXADECIMAL_UNSIGNED)\
                X(LITERAL_HEXADECIMAL_UNSIGNED_SPECIFIED)\
        X(LITERAL_OCTAL)\
            X(LITERAL_OCTAL_INTEGER)\
                X(LITERAL_OCTAL_INTEGER_SPECIFIED)\
            X(LITERAL_OCTAL_UNSIGNED)\
                X(LITERAL_OCTAL_UNSIGNED_SPECIFIED)\
        X(LITERAL_BINARY)\
            X(LITERAL_BINARY_INTEGER)\
                X(LITERAL_BINARY_INTEGER_SPECIFIED)\
            X(LITERAL_BINARY_UNSIGNED)\
                X(LITERAL_BINARY_UNSIGNED_SPECIFIED)\
    X(COMMENT)\

enum
{
#define X(name) TOKEN_TYPE_##name,
    TOKEN_TYPE_X_LIST
    TOKEN_TYPE_COUNT,
#undef X
};

extern const char* const tokenType[TOKEN_TYPE_COUNT];
typedef uint8_t TokenType;

enum
{
    TOKEN_FLAG_LEFT_EDGE_ERROR  = 1 << 0,
    TOKEN_FLAG_RIGHT_EDGE_ERROR = 1 << 1,
    TOKEN_FLAG_MASK_EDGE_ERROR = TOKEN_FLAG_LEFT_EDGE_ERROR
                               | TOKEN_FLAG_RIGHT_EDGE_ERROR,
};
typedef uint8_t TokenFlags;

typedef struct s_Token
{
    //! This is a pointer into the file memory.
    char* str;
    //! This is the index into the file data the token was found at.
    size_t index;
    size_t line;
    size_t col;
    //! All tokens referring to the same variable should have the same id.
    uint32_t id;
    //! Byte length of the token.
    uint8_t len;
    //! Between this and the `id`, all unique tokens can be matched.
    TokenType tokenType;
    //! Some error info goes in here.
    TokenFlags tokenFlags;
}
Token;

typedef struct
{
    size_t size;
    size_t count;
    Token* list; 
}
TokenList;

extern TokenList tokenListCreate (size_t size);
extern void tokenListAppend (TokenList* tokenList, Token token);
