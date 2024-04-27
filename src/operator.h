#pragma once
#include "core.h"

enum
{
    PREFIX        = 1 << 0,
    INFIX         = 1 << 1,
    SUFFIX        = 1 << 2,
    BRACKET_OPEN  = 1 << 3,
    BRACKET_CLOSE = 1 << 4,
};
typedef uint8_t OperatorFlags;

//TODO: Add precedence.
#define OPERATOR_X_LIST \
    X("!",LOGICAL_NOT,SUFFIX)\
    X("!=",NOT_EQ,SUFFIX)\
    X("#",ARG_SPEC,PREFIX)\
    X("#>",FEED,INFIX)\
    X("%",MOD,PREFIX|INFIX)\
    X("%=",MOD_ASSIGN,INFIX)\
    X("&",REF_SLASH_BIT_AND,PREFIX|INFIX)\
    X("&&",LOGICAL_AND,INFIX)\
    X("&&=",LOGICAL_AND_ASSIGN,INFIX)\
    X("&=",BIT_AND_ASSIGN,INFIX)\
    X("(",PAREN_OPEN,BRACKET_OPEN)\
    X("(*",COMMENT_OPEN,BRACKET_OPEN)\
    X(")",PAREN_CLOSE,BRACKET_CLOSE)\
    X("*",MUL,INFIX)\
    X("*=",MUL_ASSIGN,INFIX)\
    X("+",ADD,INFIX|PREFIX)\
    X("++",INCREMENT,PREFIX|SUFFIX)\
    X("+=",ADD_ASSIGN,INFIX)\
    X(",",SEP,INFIX)\
    X("-",SUB,PREFIX|INFIX)\
    X("--",DECREMENT,PREFIX|INFIX)\
    X("-=",SUB_ASSIGN,INFIX)\
    X("->",RANGE,INFIX)\
    X("->.",METHOD_RANGE,INFIX)\
    X(".",ACCESS,INFIX)\
    X("/",DIV,INFIX)\
    X("/=",DIV_ASSIGN,INFIX)\
    X(":",SPLICE,INFIX)\
    X("::",COMPILE_DEFINE,INFIX)\
    X(":=",RUN_DEFINE,INFIX)\
    X(";",END_STATEMENT,SUFFIX)\
    X("<",LESS_THAN,INFIX)\
    X("<<",SHIFT_LEFT,INFIX)\
    X("<<<",SHUNT_LEFT,SUFFIX)\
    X("<<=",SHIFT_LEFT_ASSIGN,INFIX)\
    X("<<>",POP_LEFT,SUFFIX)\
    X("<=",GREATER_THAN_OR_EQ,INFIX)\
    X("<>",BIT_REVERSE,SUFFIX)\
    X("<><",PUSH_RIGHT,INFIX)\
    X("<>>",POP_RIGHT,SUFFIX)\
    X("=",ASSIGN,INFIX)\
    X("==",IS_EQ,INFIX)\
    X("=@",DEREF_ASSIGN,INFIX)\
    X(">",GREATER_THAN,INFIX)\
    X("><",SWAP,INFIX)\
    X("><>",PUSH_LEFT,INFIX)\
    X(">=",LESS_THAN_OR_EQ,INFIX)\
    X(">>",SHIFT_RIGHT,INFIX)\
    X(">>=",SHIFT_RIGHT_ASSIGN,INFIX)\
    X(">>>",SHUNT_RIGHT,SUFFIX)\
    X("?",NULLABLE,SUFFIX)\
    X("@",DEREF,SUFFIX)\
    X("[",ARRAY_INDEX_OPEN,BRACKET_OPEN)\
    X("\\(",MACRO_OPEN,BRACKET_OPEN)\
    X("]",INDEX_CLOSE,BRACKET_CLOSE)\
    X("^",BIT_XOR,INFIX)\
    X("^=",BIT_XOR_ASSIGN,INFIX)\
    X("^^",LOGICAL_XOR,INFIX)\
    X("^^=",LOGICAL_XOR_ASSIGN,INFIX)\
    X("{",BLOCK_OPEN,BRACKET_OPEN)\
    X("|",BIT_OR,INFIX)\
    X("|=",BIT_OR_ASSIGN,INFIX)\
    X("|[",BIT_INDEX_OPEN,BRACKET_OPEN)\
    X("||",LOGICAL_OR,INFIX)\
    X("||=",LOGICAL_OR_ASSIGN,INFIX)\
    X("}",BLOCK_CLOSE,BRACKET_CLOSE)\
    X("~",BIT_NOT,SUFFIX)\

enum
{
#define X(str,name,pos) OPERATOR_##name,
    OPERATOR_X_LIST
    OPERATOR_COUNT,
#undef X
};
typedef uint8_t Operator;

extern const char* const operators[OPERATOR_COUNT];
extern const uint8_t operatorFlags[OPERATOR_COUNT];
extern bool operatorFind (Operator searchStart, uint8_t opLen, char* opStr, Operator* result);

