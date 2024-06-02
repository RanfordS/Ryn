#pragma once

#define KEYWORD_X_LIST \
    X(BREAK,break)\
    X(CASE,case)\
    X(CONTINUE,continue)\
    X(ELSE,else)\
    X(ELSEIF,elseif)\
    X(FOR,for)\
    X(GOTO,goto)\
    X(IF,if)\
    X(LABEL,label)\
    X(MATCH,match)\
    X(PROVIDING,providing)\
    X(RETURN,return)\
    X(USING,using)\
    X(WHILE,while)\

enum
{
#define X(name,keyword) KEYWORD_##name,
    KEYWORD_X_LIST
    KEYWORD_COUNT,
#undef X
};

extern const char* const keywords[KEYWORD_COUNT];

