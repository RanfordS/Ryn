#pragma once
#include "core.h"

enum
{
    CHAR_CLASS_INVALID    = 0,
    CHAR_FLAG_LETTER      = 1 << 0,
    CHAR_FLAG_UPPER       = 1 << 1,
    CHAR_FLAG_UNDERSCORE  = 1 << 2,
    CHAR_FLAG_BRACKET     = 1 << 3,
    CHAR_FLAG_WHITE_SPACE = 1 << 4,
    CHAR_FLAG_NUMBER      = 1 << 5,
    CHAR_FLAG_QUOTE       = 1 << 6,
    CHAR_FLAG_OPERATOR    = 1 << 7,

    CHAR_MASK_NOUN
        = CHAR_FLAG_LETTER
        | CHAR_FLAG_UPPER
        | CHAR_FLAG_NUMBER
        | CHAR_FLAG_UNDERSCORE,
};
typedef U8 CharClass;

extern CharClass charClassifier (char c);
