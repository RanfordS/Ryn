#pragma once
#include "core.h"

enum
{
    OPERATOR_POSITION_PREFIX,
    OPERATOR_POSITION_INFIX,
    OPERATOR_POSITION_SUFFIX,
    OPERATOR_POSITION_BRACKET_OPEN,
    OPERATOR_POSITION_BRACKET_CLOSE,
};
typedef uint8_t OperatorPosition;

enum
{
    OPERATOR_CONTEXT_ALWAYS,
    OPERATOR_CONTEXT_TYPESPEC,
    OPERATOR_CONTEXT_EXPRESSION,
    OPERATOR_CONTEXT_MATCH,
    OPERATOR_CONTEXT_FOR,
};
typedef uint8_t OperatorContext;

enum
{
    OPERATOR_PRECEDENCE_NA,
    OPERATOR_PRECEDENCE_SUFFIX,
    OPERATOR_PRECEDENCE_PREFIX,
    OPERATOR_PRECEDENCE_ARGSPEC,
    OPERATOR_PRECEDENCE_ARGUMENT,
    OPERATOR_PRECEDENCE_ARGFEED,
    //! The "DM" part of BODMAS.
    OPERATOR_PRECEDENCE_MATH_DM,
    //! The "AS" part of BODMAS.
    OPERATOR_PRECEDENCE_MATH_AS,
    OPERATOR_PRECEDENCE_SHIFT,
    OPERATOR_PRECEDENCE_RANGE,
    OPERATOR_PRECEDENCE_COMPARE,
    OPERATOR_PRECEDENCE_BIT_AND,
    OPERATOR_PRECEDENCE_BIT_XOR,
    OPERATOR_PRECEDENCE_BIT_OR,
    OPERATOR_PRECEDENCE_LOGICAL_AND,
    OPERATOR_PRECEDENCE_LOGICAL_XOR,
    OPERATOR_PRECEDENCE_LOGICAL_OR,
    OPERATOR_PRECEDENCE_ARRAY,
    OPERATOR_PRECEDENCE_SLICE,
    OPERATOR_PRECEDENCE_COMMA,
    OPERATOR_PRECEDENCE_ASSIGN,
    OPERATOR_PRECEDENCE_SEMICOLON,
    OPERATOR_PRECEDENCE_COUNT,
};
typedef uint8_t OperatorPrecedence;

// This list must be kept in order!
// Sub-sequences must be immediately before their extensions
// e.g., `&` preceeds `&=` and `&&`
// Vim: 3jV}k:sort <- yiw@"<CR> 
// String, Enum, Position, Context, Precedence
#define OPERATOR_X_LIST \
    X("!",LOGICAL_NOT,SUFFIX,EXPRESSION,SUFFIX)\
    X("!=",NOT_EQ,SUFFIX,EXPRESSION,SUFFIX)\
    X("#",ARG_SPEC,PREFIX,EXPRESSION,SUFFIX)\
    X("#>",FEED,INFIX,EXPRESSION,ARGFEED)\
    X("$",REFERENCE,PREFIX,TYPESPEC,PREFIX)\
    X("$",TAKE_REFERENCE,SUFFIX,EXPRESSION,SUFFIX)\
    X("%",MODULUS,INFIX,EXPRESSION,MATH_DM)\
    X("%",MUTABLE,PREFIX,TYPESPEC,PREFIX)\
    X("%=",MOD_ASSIGN,INFIX,EXPRESSION,ASSIGN)\
    X("&",BIT_AND,INFIX,EXPRESSION,BIT_AND)\
    X("&&",LOGICAL_AND,INFIX,EXPRESSION,LOGICAL_AND)\
    X("&&=",LOGICAL_AND_ASSIGN,INFIX,EXPRESSION,ASSIGN)\
    X("&=",BIT_AND_ASSIGN,INFIX,EXPRESSION,ASSIGN)\
    X("(",PAREN_OPEN,BRACKET_OPEN,ALWAYS,SUFFIX)\
    X("(*",COMMENT_OPEN,BRACKET_OPEN,ALWAYS,SUFFIX)\
    X(")",PAREN_CLOSE,BRACKET_CLOSE,ALWAYS,NA)\
    X("*",MUL,INFIX,EXPRESSION,MATH_DM)\
    X("*=",MUL_ASSIGN,INFIX,EXPRESSION,ASSIGN)\
    X("+",ADD,INFIX,EXPRESSION,MATH_AS)\
    X("+",UNARY_PLUS,PREFIX,EXPRESSION,PREFIX)\
    X("+=",ADD_ASSIGN,INFIX,EXPRESSION,ASSIGN)\
    X(",",SEP,INFIX,EXPRESSION,COMMA)\
    X("-",SUB,INFIX,EXPRESSION,MATH_AS)\
    X("-",UNARY_MINUS,PREFIX,EXPRESSION,PREFIX)\
    X("-<",REV_RANGE,INFIX,FOR,RANGE)\
    X("-=",SUB_ASSIGN,INFIX,EXPRESSION,ASSIGN)\
    X("->",CASE_START,INFIX,MATCH,SEMICOLON)\
    X("->",RANGE,INFIX,FOR,RANGE)\
    X("->",RETURN_SPEC,INFIX,TYPESPEC,RANGE)\
    X("->.",METHOD_RANGE,INFIX,FOR,RANGE)\
    X(".",ACCESS,INFIX,EXPRESSION,SUFFIX)\
    X("..",CONCAT,INFIX,EXPRESSION,ARRAY)\
    X("/",DIV,INFIX,EXPRESSION,MATH_DM)\
    X("/=",DIV_ASSIGN,INFIX,EXPRESSION,ASSIGN)\
    X(":",SLICE,INFIX,EXPRESSION,SLICE)\
    X("::",COMPILE_DEFINE,INFIX,EXPRESSION,ASSIGN)\
    X(":=",RUN_DEFINE,INFIX,EXPRESSION,ASSIGN)\
    X(";",END_STATEMENT,SUFFIX,EXPRESSION,SEMICOLON)\
    X("<",LESS_THAN,INFIX,EXPRESSION,COMPARE)\
    X("<<",SHIFT_LEFT,INFIX,EXPRESSION,SHIFT)\
    X("<<<",SHUNT_LEFT,SUFFIX,EXPRESSION,SUFFIX)\
    X("<<=",SHIFT_LEFT_ASSIGN,INFIX,EXPRESSION,ASSIGN)\
    X("<<>",POP_LEFT,SUFFIX,EXPRESSION,ARRAY)\
    X("<=",GREATER_THAN_OR_EQ,INFIX,EXPRESSION,COMPARE)\
    X("<>",BIT_REVERSE,SUFFIX,EXPRESSION,SUFFIX)\
    X("<><",PUSH_RIGHT,INFIX,EXPRESSION,ARRAY)\
    X("<>>",POP_RIGHT,SUFFIX,EXPRESSION,ARRAY)\
    X("=",ASSIGN,INFIX,EXPRESSION,ASSIGN)\
    X("==",IS_EQ,INFIX,EXPRESSION,COMPARE)\
    X("=@",DEREF_ASSIGN,INFIX,EXPRESSION,ASSIGN)\
    X(">",GREATER_THAN,INFIX,EXPRESSION,COMPARE)\
    X("><",SWAP,INFIX,EXPRESSION,ASSIGN)\
    X("><>",PUSH_LEFT,INFIX,EXPRESSION,ARRAY)\
    X(">=",LESS_THAN_OR_EQ,INFIX,EXPRESSION,COMPARE)\
    X(">>",SHIFT_RIGHT,INFIX,EXPRESSION,SHIFT)\
    X(">>=",SHIFT_RIGHT_ASSIGN,INFIX,EXPRESSION,ASSIGN)\
    X(">>>",SHUNT_RIGHT,SUFFIX,EXPRESSION,SUFFIX)\
    X("?",NULLABLE,PREFIX,TYPESPEC,PREFIX)\
    X("@",DEREF,SUFFIX,EXPRESSION,SUFFIX)\
    X("[",ARRAY_INDEX_OPEN,BRACKET_OPEN,EXPRESSION,SUFFIX)\
    X("[",ARRAY_SPECIFIER_OPEN,BRACKET_OPEN,TYPESPEC,PREFIX)\
    X("\\(",MACRO_OPEN,BRACKET_OPEN,ALWAYS,PREFIX)\
    X("]",INDEX_CLOSE,BRACKET_CLOSE,ALWAYS,NA)\
    X("^",BIT_XOR,INFIX,EXPRESSION,BIT_XOR)\
    X("^=",BIT_XOR_ASSIGN,INFIX,EXPRESSION,ASSIGN)\
    X("^^",LOGICAL_XOR,INFIX,EXPRESSION,LOGICAL_XOR)\
    X("^^=",LOGICAL_XOR_ASSIGN,INFIX,EXPRESSION,ASSIGN)\
    X("{",BLOCK_OPEN,BRACKET_OPEN,EXPRESSION,SUFFIX)\
    X("|",BIT_OR,INFIX,EXPRESSION,BIT_OR)\
    X("|=",BIT_OR_ASSIGN,INFIX,EXPRESSION,ASSIGN)\
    X("|[",BIT_INDEX_OPEN,BRACKET_OPEN,EXPRESSION,SUFFIX)\
    X("||",LOGICAL_OR,INFIX,EXPRESSION,LOGICAL_OR)\
    X("||=",LOGICAL_OR_ASSIGN,INFIX,EXPRESSION,ASSIGN)\
    X("}",BLOCK_CLOSE,BRACKET_CLOSE,ALWAYS,NA)\
    X("~",BIT_NOT,SUFFIX,EXPRESSION,SUFFIX)\

enum
{
#define X(str,name,pos,loc,prec) OPERATOR_##name,
    OPERATOR_X_LIST
    OPERATOR_COUNT,
#undef X
};
typedef uint8_t Operator;

extern const char* const operators[OPERATOR_COUNT];
extern const OperatorPosition operatorPositions[OPERATOR_COUNT];
extern const OperatorPrecedence operatorPrecedences[OPERATOR_COUNT];
extern bool operatorFind (Operator searchStart, uint8_t opLen, const char* opStr, Operator* result);

