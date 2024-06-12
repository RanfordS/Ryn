#pragma once
#include "core.h"
#include "listify.h"
#include "rynerror.h"

/*
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
*/

#define TOKEN_TYPE_X_LIST \
    X(NONE)\
    X(START_OF_FILE)\
    X(ERRONEOUS_CLOSE)\
    X(END_OF_FILE)\
    X(NOUN)\
        X(TYPE)\
        X(GENERIC_TYPE)\
        X(KEYWORD)\
    X(OPERATOR)\
    X(LITERAL_NUMBER)\
        X(LITERAL_ZERO)\
        X(LITERAL_BINARY)\
        X(LITERAL_OCTAL)\
        X(LITERAL_HEXADECIMAL)\
        X(LITERAL_DECIMAL)\
        X(LITERAL_EXPONENT)\
        X(LITERAL_NUMBER_SPECIFIED)\
        X(LITERAL_ZERO_SPECIFIED)\
        X(LITERAL_BINARY_SPECIFIED)\
        X(LITERAL_OCTAL_SPECIFIED)\
        X(LITERAL_HEXADECIMAL_SPECIFIED)\
        X(LITERAL_DECIMAL_SPECIFIED)\
        X(LITERAL_EXPONENT_SPECIFIED)\
    X(LITERAL_CHAR)\
    X(LITERAL_STRING)\
    X(COMMENT)\
    X(INVALID)\

/*! \var TOKEN_TYPE_OPERATOR
 *  \brief The leftIndex and rightIndex are ...
 */
enum
{
#define X(name) TOKEN_TYPE_##name,
    TOKEN_TYPE_X_LIST
    TOKEN_TYPE_COUNT,
#undef X
};
typedef uint8_t TokenType;

extern const char* const tokenType[TOKEN_TYPE_COUNT];

typedef struct s_Token
{
    //! This is a pointer into the file memory.
    const char* string;
    //! This is the index into the file data the token was found at.
    size_t index;
    size_t line;
    size_t column;
    //! All tokens referring to the same variable should have the same id.
    uint32_t ident;
    //! Byte length of the token.
    uint8_t length;
    //! Between this and the `id`, all unique tokens can be matched.
    TokenType type;
    //! For operators, index of the left-hand argument.
    size_t leftIndex;
    //! For operators, index of the right-hand argument.
    size_t rightIndex;
    //! Index of the next item in a list.
    size_t nextIndex;
    //! 
    size_t varType;
    RynError error;
    bool parsed;
}
Token;

LISTIFY_H(Token)

extern void tokenListPrint (const char* source, const TokenList* tokenList);

// EOF //
