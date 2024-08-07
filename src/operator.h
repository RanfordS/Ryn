#pragma once
#include "core.h"
enum
{
    OPERAND_FLAG_LHS_POSSIBLE  = 1 << 0,
    OPERAND_FLAG_LHS_REQUIRED  = 1 << 1,
    OPERAND_FLAG_RHS_POSSIBLE  = 1 << 2,
    OPERAND_FLAG_RHS_REQUIRED  = 1 << 3,
    OPERAND_FLAG_OPEN_BRACKET  = 1 << 4,
    OPERAND_FLAG_CLOSE_BRACKET = 1 << 5,

    OPERAND_MASK_POSSIBLE
        = OPERAND_FLAG_LHS_POSSIBLE
        | OPERAND_FLAG_RHS_POSSIBLE,

    OPERAND_MASK_REQUIRED
        = OPERAND_FLAG_LHS_REQUIRED
        | OPERAND_FLAG_RHS_REQUIRED,

    OPERAND_MASK_BRACKET
        = OPERAND_FLAG_OPEN_BRACKET
        | OPERAND_FLAG_CLOSE_BRACKET,

    OPERAND_LHS_NONE      = 0,
    OPERAND_LHS_OPTIONAL  = OPERAND_FLAG_LHS_POSSIBLE,
    OPERAND_LHS_REQUIRED  = OPERAND_FLAG_LHS_POSSIBLE | OPERAND_FLAG_LHS_REQUIRED,
    OPERAND_MASK_LHS_SEARCH
        = OPERAND_FLAG_LHS_POSSIBLE
        | OPERAND_FLAG_LHS_REQUIRED, 

    OPERAND_RHS_NONE      = 0,
    OPERAND_RHS_OPTIONAL  = OPERAND_FLAG_RHS_POSSIBLE,
    OPERAND_RHS_REQUIRED  = OPERAND_FLAG_RHS_POSSIBLE | OPERAND_FLAG_RHS_REQUIRED,
    OPERAND_MASK_RHS_SEARCH
        = OPERAND_FLAG_RHS_POSSIBLE
        | OPERAND_FLAG_RHS_REQUIRED, 

    OPERAND_LHS_ACTIVE_BRACKET = OPERAND_LHS_REQUIRED | OPERAND_FLAG_OPEN_BRACKET,
    OPERAND_LHS_BRACKET = OPERAND_FLAG_OPEN_BRACKET,
    OPERAND_RHS_BRACKET = OPERAND_FLAG_CLOSE_BRACKET,
};
typedef U8 OperatorFlags;


enum
{
    OPERATOR_CONTEXT_ALWAYS,
    OPERATOR_CONTEXT_TYPESPEC,
    OPERATOR_CONTEXT_EXPRESSION,
    OPERATOR_CONTEXT_MATCH,
    OPERATOR_CONTEXT_FOR,
};
typedef U8 OperatorContext;

enum
{
    OPERATOR_PRECEDENCE_NA,
    OPERATOR_PRECEDENCE_SEMICOLON,
    OPERATOR_PRECEDENCE_ASSIGN,
    OPERATOR_PRECEDENCE_COMMA,
    OPERATOR_PRECEDENCE_SLICE,
    OPERATOR_PRECEDENCE_ARRAY,
    OPERATOR_PRECEDENCE_LOGICAL_OR,
    OPERATOR_PRECEDENCE_LOGICAL_XOR,
    OPERATOR_PRECEDENCE_LOGICAL_AND,
    OPERATOR_PRECEDENCE_COMPARE,
    OPERATOR_PRECEDENCE_RANGE,
    OPERATOR_PRECEDENCE_BIT_OR,
    OPERATOR_PRECEDENCE_BIT_XOR,
    OPERATOR_PRECEDENCE_BIT_AND,
    OPERATOR_PRECEDENCE_SHIFT,
    //! The "AS" part of BODMAS.
    OPERATOR_PRECEDENCE_MATH_AS,
    //! The "DM" part of BODMAS.
    OPERATOR_PRECEDENCE_MATH_DM,
    OPERATOR_PRECEDENCE_ARGFEED,
    OPERATOR_PRECEDENCE_ARGUMENT,
    OPERATOR_PRECEDENCE_ARGSPEC,
    OPERATOR_PRECEDENCE_PREFIX,
    OPERATOR_PRECEDENCE_SUFFIX,
    OPERATOR_PRECEDENCE_COUNT,
};
typedef U8 OperatorPrecedence;

extern const char* precedenceNames[OPERATOR_PRECEDENCE_COUNT];

// This list must be kept in order!
// Sub-sequences must be immediately before their extensions
// e.g., `&` preceeds `&=` and `&&`
// Vim: 3jV}k:sort <- yiw@"<CR> 
// String, Enum, LHS Type, RHS Type, Context, Precedence
#define OPERATOR_X_LIST \
    X("!",LOGICAL_NOT,REQUIRED,NONE,EXPRESSION,SUFFIX)\
    X("!=",NOT_EQ,REQUIRED,REQUIRED,EXPRESSION,SUFFIX)\
    X("#",ARG_SPEC,NONE,REQUIRED,EXPRESSION,SUFFIX)\
    X("#>",FEED,REQUIRED,REQUIRED,EXPRESSION,ARGFEED)\
    X("$",REFERENCE,NONE,REQUIRED,TYPESPEC,PREFIX)\
    X("$",TAKE_REFERENCE,REQUIRED,NONE,EXPRESSION,SUFFIX)\
    X("%",MODULUS,REQUIRED,REQUIRED,EXPRESSION,MATH_DM)\
    X("%",MUTABLE,NONE,REQUIRED,TYPESPEC,PREFIX)\
    X("%=",MOD_ASSIGN,REQUIRED,REQUIRED,EXPRESSION,ASSIGN)\
    X("&",BIT_AND,REQUIRED,REQUIRED,EXPRESSION,BIT_AND)\
    X("&&",LOGICAL_AND,REQUIRED,REQUIRED,EXPRESSION,LOGICAL_AND)\
    X("&&=",LOGICAL_AND_ASSIGN,REQUIRED,REQUIRED,EXPRESSION,ASSIGN)\
    X("&=",BIT_AND_ASSIGN,REQUIRED,REQUIRED,EXPRESSION,ASSIGN)\
    X("(",PAREN_OPEN,BRACKET,NONE,ALWAYS,SUFFIX)\
    X("(*",COMMENT_OPEN,BRACKET,NONE,ALWAYS,SUFFIX)\
    X(")",PAREN_CLOSE,NONE,BRACKET,ALWAYS,NA)\
    X("*",MUL,REQUIRED,REQUIRED,EXPRESSION,MATH_DM)\
    X("*=",MUL_ASSIGN,REQUIRED,REQUIRED,EXPRESSION,ASSIGN)\
    X("+",ADD,OPTIONAL,REQUIRED,EXPRESSION,MATH_AS)\
    X("+=",ADD_ASSIGN,REQUIRED,REQUIRED,EXPRESSION,ASSIGN)\
    X(",",SEP,REQUIRED,REQUIRED,EXPRESSION,COMMA)\
    X("-",SUB,OPTIONAL,REQUIRED,EXPRESSION,MATH_AS)\
    X("-<",REV_RANGE,REQUIRED,REQUIRED,FOR,RANGE)\
    X("-=",SUB_ASSIGN,REQUIRED,REQUIRED,EXPRESSION,ASSIGN)\
    X("->",CASE_START,REQUIRED,REQUIRED,MATCH,SEMICOLON)\
    X("->",RANGE,REQUIRED,REQUIRED,FOR,RANGE)\
    X("->",RETURN_SPEC,REQUIRED,REQUIRED,TYPESPEC,RANGE)\
    X("->.",METHOD_RANGE,REQUIRED,REQUIRED,FOR,RANGE)\
    X(".",ACCESS,REQUIRED,REQUIRED,EXPRESSION,SUFFIX)\
    X("..",CONCAT,REQUIRED,REQUIRED,EXPRESSION,ARRAY)\
    X("..=",CONCAT_ASSIGN,REQUIRED,REQUIRED,EXPRESSION,ARRAY)\
    X("/",DIV,REQUIRED,REQUIRED,EXPRESSION,MATH_DM)\
    X("/=",DIV_ASSIGN,REQUIRED,REQUIRED,EXPRESSION,ASSIGN)\
    X(":",SLICE,REQUIRED,REQUIRED,EXPRESSION,SLICE)\
    X("::",COMPILE_DEFINE,REQUIRED,REQUIRED,EXPRESSION,ASSIGN)\
    X(":=",RUN_DEFINE,REQUIRED,REQUIRED,EXPRESSION,ASSIGN)\
    X(";",END_STATEMENT,OPTIONAL,OPTIONAL,EXPRESSION,SEMICOLON)\
    X("<",LESS_THAN,REQUIRED,REQUIRED,EXPRESSION,COMPARE)\
    X("<<",SHIFT_LEFT,REQUIRED,REQUIRED,EXPRESSION,SHIFT)\
    X("<<<",SHUNT_LEFT,REQUIRED,NONE,EXPRESSION,SUFFIX)\
    X("<<=",SHIFT_LEFT_ASSIGN,REQUIRED,REQUIRED,EXPRESSION,ASSIGN)\
    X("<<>",POP_LEFT,REQUIRED,NONE,EXPRESSION,ARRAY)\
    X("<=",GREATER_THAN_OR_EQ,REQUIRED,REQUIRED,EXPRESSION,COMPARE)\
    X("<>",BIT_REVERSE,REQUIRED,NONE,EXPRESSION,SUFFIX)\
    X("<><",PUSH_RIGHT,REQUIRED,REQUIRED,EXPRESSION,ARRAY)\
    X("<>>",POP_RIGHT,REQUIRED,NONE,EXPRESSION,ARRAY)\
    X("=",ASSIGN,REQUIRED,REQUIRED,EXPRESSION,ASSIGN)\
    X("==",IS_EQ,REQUIRED,REQUIRED,EXPRESSION,COMPARE)\
    X("=@",DEREF_ASSIGN,REQUIRED,REQUIRED,EXPRESSION,ASSIGN)\
    X(">",GREATER_THAN,REQUIRED,REQUIRED,EXPRESSION,COMPARE)\
    X("><",SWAP,REQUIRED,REQUIRED,EXPRESSION,ASSIGN)\
    X("><>",PUSH_LEFT,REQUIRED,REQUIRED,EXPRESSION,ARRAY)\
    X(">=",LESS_THAN_OR_EQ,REQUIRED,REQUIRED,EXPRESSION,COMPARE)\
    X(">>",SHIFT_RIGHT,REQUIRED,REQUIRED,EXPRESSION,SHIFT)\
    X(">>=",SHIFT_RIGHT_ASSIGN,REQUIRED,REQUIRED,EXPRESSION,ASSIGN)\
    X(">>>",SHUNT_RIGHT,REQUIRED,NONE,EXPRESSION,SUFFIX)\
    X("?",NULLABLE,NONE,REQUIRED,TYPESPEC,PREFIX)\
    X("@",DEREF,REQUIRED,NONE,EXPRESSION,SUFFIX)\
    X("[",ARRAY_INDEX_OPEN,ACTIVE_BRACKET,NONE,EXPRESSION,SUFFIX)\
    X("[",ARRAY_SPECIFIER_OPEN,ACTIVE_BRACKET,NONE,TYPESPEC,PREFIX)\
    X("\\(",MACRO_OPEN,BRACKET,NONE,ALWAYS,PREFIX)\
    X("]",INDEX_CLOSE,NONE,BRACKET,ALWAYS,NA)\
    X("^",BIT_XOR,REQUIRED,REQUIRED,EXPRESSION,BIT_XOR)\
    X("^=",BIT_XOR_ASSIGN,REQUIRED,REQUIRED,EXPRESSION,ASSIGN)\
    X("^^",LOGICAL_XOR,REQUIRED,REQUIRED,EXPRESSION,LOGICAL_XOR)\
    X("^^=",LOGICAL_XOR_ASSIGN,REQUIRED,REQUIRED,EXPRESSION,ASSIGN)\
    X("{",BLOCK_OPEN,BRACKET,NONE,EXPRESSION,SUFFIX)\
    X("|",BIT_OR,REQUIRED,REQUIRED,EXPRESSION,BIT_OR)\
    X("|=",BIT_OR_ASSIGN,REQUIRED,REQUIRED,EXPRESSION,ASSIGN)\
    X("|[",BIT_INDEX_OPEN,ACTIVE_BRACKET,NONE,EXPRESSION,SUFFIX)\
    X("||",LOGICAL_OR,REQUIRED,REQUIRED,EXPRESSION,LOGICAL_OR)\
    X("||=",LOGICAL_OR_ASSIGN,REQUIRED,REQUIRED,EXPRESSION,ASSIGN)\
    X("}",BLOCK_CLOSE,NONE,BRACKET,ALWAYS,NA)\
    X("~",BIT_NOT,REQUIRED,NONE,EXPRESSION,SUFFIX)\

enum
{
#define X(str,name,lhsType,rhsType,loc,prec) OPERATOR_##name,
    OPERATOR_X_LIST
    OPERATOR_COUNT,
#undef X
};
typedef U8 Operator;

extern const char* const operators[OPERATOR_COUNT];
extern const OperatorFlags operatorFlags[OPERATOR_COUNT];
extern const OperatorPrecedence operatorPrecedences[OPERATOR_COUNT];
extern bool operatorFind (Operator searchStart, U8 opLen, const char* opStr, Operator* result);
extern bool operatorMatchContext (Operator searchStart, OperatorContext context, Operator* result);
extern bool operatorMatchFlagsAndContext (Operator searchStart, OperatorFlags flags, OperatorContext context, Operator* result);

