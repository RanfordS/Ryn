#pragma once
#include "core.h"

#define KEYWORD_X_LIST \
    X(BREAK,break,false)\
    X(CASE,case,false)\
    X(CONTINUE,continue,false)\
    X(ELSE,else,true)\
    X(ELSEIF,elseif,true)\
    X(FOR,for,true)\
    X(GOTO,goto,false)\
    X(IF,if,true)\
    X(LABEL,label,false)\
    X(MATCH,match,true)\
    X(PROVIDING,providing,false)\
    X(RETURN,return,false)\
    X(USING,using,false)\
    X(WHILE,while,true)\
    X(ENUM,Enum,true)\
    X(FUNC,Func,true)\
    X(STRUCT,Struct,true)\
    X(UNION,Union,true)\

enum
{
#define X(name,keyword,hasBlock) KEYWORD_##name,
    KEYWORD_X_LIST
    KEYWORD_COUNT,
#undef X
};

extern const char* const keywords[KEYWORD_COUNT];
extern const bool keywordHasBlock[KEYWORD_COUNT];

