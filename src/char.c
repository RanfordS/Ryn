#include "char.h"

CharClass charClassifier (char c)
{
    switch (c)
    {
        case '_':
            return CHAR_FLAG_LETTER | CHAR_FLAG_UNDERSCORE;

        case '"':
        case '\'':
            return CHAR_FLAG_QUOTE;

        case ' ':
        case '\t':
        case '\r':
        case '\n':
            return CHAR_FLAG_WHITE_SPACE;

        case '{': case '}':
        case '[': case ']':
        case '(': case ')':
            return CHAR_FLAG_OPERATOR | CHAR_FLAG_BRACKET;

        default: break;
    }
    
    if ((c <= 0x1F)
    ||  (0x7F <= c))
    {
        return CHAR_CLASS_INVALID;
    }

    if (('A' <= c) & (c <= 'Z'))
    {
        return CHAR_FLAG_LETTER | CHAR_FLAG_UPPER;
    }
    if (('a' <= c) & (c <= 'z'))
    {
        return CHAR_FLAG_LETTER;
    }

    if (('0' <= c) & (c <= '9'))
    {
        return CHAR_FLAG_NUMBER;
    }

    return CHAR_FLAG_OPERATOR;
}
