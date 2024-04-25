#pragma once

#define KEYWORD_X_LIST \
    X(USING,using)\
    X(PROVIDING,providing)\
    X(IF,if)\
    X(ELSEIF,elseif)\
    X(ELSE,else)\
    X(FOR,for)\
    X(WHILE,while)\
    X(MATCH,match)\
    X(RETURN,return)\
    X(CONTINUE,continue)\
    X(BREAK,break)\

enum
{
#define X(name,keyword) KEYWORD_##name,
    KEYWORD_X_LIST
    KEYWORD_COUNT,
#undef X
};

extern const char* const keywords[KEYWORD_COUNT];

