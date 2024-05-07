#include "lexer.h"
#include "char.h"
#include "stringlib.h"
#include "token.h"
#include "keyword.h"

typedef struct
{
    size_t index;
    Operator operator;
}
BracketItem;

LISTIFY_H(BracketItem)
LISTIFY_C(BracketItem)

size_t bracketer (TokenList* list)
{
    printf ("Running bracketer\n");
    BracketItemList brackets = createBracketItemList (16);

    for (size_t i = 0; i < list->count; ++i)
    {
        printf ("Considering token %llu\n", i);
        Token* token = list->data + i;
        if (token->tokenType != TOKEN_TYPE_OPERATOR) { continue; }
        printf ("It's an operator, investigate\n");

        bool shouldClose = false;

        switch (token->ident)
        {
            case OPERATOR_ARRAY_INDEX_OPEN:
            case OPERATOR_BIT_INDEX_OPEN:
            case OPERATOR_BLOCK_OPEN:
            case OPERATOR_MACRO_OPEN:
            case OPERATOR_PAREN_OPEN:
            {   BracketItem item =
                {   .index = i,
                    .operator = token->ident, };
                appendBracketItemList (&brackets, item);
                printf ("A new bracket is pushed to the stack\n");
            }   break;

            case OPERATOR_BLOCK_CLOSE:
            case OPERATOR_INDEX_CLOSE:
            case OPERATOR_PAREN_CLOSE:
                shouldClose = true;
                printf ("A close bracket was encountered\n");
                break;
        }
        
        if (!shouldClose)
        {   continue; }

        if (brackets.count == 0)
        {   return i; }
        BracketItem lhsBracketItem = brackets.data[--brackets.count];

        Operator rhsExpected;

        switch (lhsBracketItem.operator)
        {
            case OPERATOR_ARRAY_INDEX_OPEN:
            case OPERATOR_BIT_INDEX_OPEN:
                rhsExpected = OPERATOR_INDEX_CLOSE;
                break;

            case OPERATOR_BLOCK_OPEN:
                rhsExpected = OPERATOR_BLOCK_CLOSE;
                break;

            case OPERATOR_MACRO_OPEN:
            case OPERATOR_PAREN_OPEN:
                rhsExpected = OPERATOR_PAREN_CLOSE;
                break;
        }

        if (token->ident != rhsExpected)
        {   return i; }

        Token* lhsToken = list->data + lhsBracketItem.index;
        lhsToken->rightIndex = i;
        token->leftIndex = lhsBracketItem.index;
    }
    return list->count;
}



uint8_t lexNumberNonZero (const char* string, uint8_t length)
{
    if (length == 0)
    {   return 0; }

    if (string[0] < '1' && '9' < string[0])
    {   return 0; }

    for (uint8_t i = 1; i < length; ++i)
    {
        CharClass class = charClassifier (string[i]);
        if (class & CHAR_FLAG_NUMBER)
        {   continue; }
        return i;
    }
    return length;
}

uint8_t lexBinary (const char* string, uint8_t length)
{
    for (uint8_t i = 0; i < length; ++i)
    {
        char c = string[i];
        if (c == '0'
        ||  c == '1')
        {   continue; }
        return i;
    }
    return length;
}

uint8_t lexOctal (const char* string, uint8_t length)
{
    for (uint8_t i = 0; i < length; ++i)
    {
        char c = string[i];
        if ('0' <= c && c <= '7')
        {   continue; }
        return i;
    }
    return length;
}

uint8_t lexHexadecimal (const char* string, uint8_t length)
{
    for (uint8_t i = 0; i < length; ++i)
    {
        char c = string[i];
        if (('0' <= c && c <= '9')
        ||  ('A' <= c && c <= 'F'))
        {   continue; }
        return i;
    }
    return length;
}

uint8_t lexDecimal (const char* string, uint8_t length)
{
    for (uint8_t i = 0; i < length; ++i)
    {
        if (charClassifier (string[i]) & CHAR_FLAG_NUMBER)
        {   continue; }
        return i;
    }
    return length;
}

uint8_t lexSpecifier (const char* string, uint8_t length)
{
    switch (string[0])
    {
        case 'u':
        case 'i':
        case 'f':
        case 'd':
            break;

        default:
            return 0;
    }
    return 1 + lexNumberNonZero (string + 1, length - 1);
}

/*  ╭Head────────────────╮ ╭Tail──────────╮ ╭Exponent───────────────╮ ╭Specifier────────────────╮  *\
╟─── 0 ─╮─ b ─╭─╮─ 0 ─╮───╭───────────────╭─╮───────────────────────╭─╮───────╭─────────────────╭───╢
║       │     │ ╰─ 1 ─┤   │               │ ╰─ e ─╮─────╭─╭──0-9──╮─╯ ├─ i ─╭─╯─ 1-9 ─╭───────╮─╯   ║
║       │     ╰──────◄╯   │               │       ├─ + ─┤ ╰──────◄╯   ├─ u ─┤         ╰─ 0-9 ◄╯     ║
║       ├─ o ─╭─ 0-7 ─╮───┤               │       ╰─ - ─╯             ├─ f ─┤                       ║
║       │     ╰──────◄╯   │               │                           ╰─ d ─╯                       ║
║       ├─ x ─╭─╮─ 0-9 ─╮─┤               │                                                         ║
║       │     │ ╰─ A-F ─┤ │               │                                                         ║
║       │     ╰────────◄╯ │               │                                                         ║
║       ╰───────────╭─────╯─ . ─╭─ 0-9 ─╮─╯                                                         ║
╟─── 1-9 ─╭───────╮─╯           ╰──────◄╯                                                           ║
║         ╰─ 0-9 ◄╯                                                                                 ║
\*                                                                                                 */

uint8_t lexNumberHead (const char* string, uint8_t length)
{
    uint8_t nonZero = lexNumberNonZero (string, length);
    if (nonZero != 0)
    {   return nonZero; }

    if (nonZero == 1)
    {   return 1; }
    if (length == 2)
    {   return 0; }

    switch (string[1])
    {
        case 'b':
            return 2 + lexBinary (string + 2, length - 2);

        case 'o':
            return 2 + lexOctal (string + 2, length - 2);

        case 'x':
            return 2 + lexHexadecimal (string + 2, length - 2);

        default:
            return 0;
    }
}

uint8_t lexNumberTail (const char* string, uint8_t length)
{
    if (length < 2)
    {   return 0; }

    if (string[0] != '.')
    {   return 0; }

    return 1 + lexDecimal (string + 1, length - 1);
}

uint8_t lexNumberExponent (const char* string, uint8_t length)
{
    if (length < 2)
    {   return 0; }

    if (string[0] != 'e')
    {   return 0; }

    if (string[1] == '-'
    ||  string[1] == '+')
    {
        if (length < 3)
        {   return 0; }

        return 2 + lexDecimal (string + 2, length - 2);
    }
    return 1 + lexDecimal (string + 1, length - 1);
}

uint8_t lexNumberSpecifier (const char* string, uint8_t length)
{
    if (length == 0)
    {   return 0; }

    switch (string[0])
    {
        case 'u':
        case 'i':
        case 'f':
        case 'd':
            break;

        default:
            return 0;
    }

    if (length == 1)
    {   return 1; }

    return 1 + lexNumberNonZero (string + 1, length - 1);
}

uint8_t lexLiteralNumber (Token* token)
{
    char*   string = token->string;
    uint8_t length = token->length;

    uint8_t pos = 0;
    uint8_t headLength = lexNumberHead (string, length);
    pos += headLength;

    uint8_t tailLength = lexNumberTail (string + pos, length - pos);
    pos += tailLength;
    
    uint8_t exponentLength = lexNumberExponent (string + pos, length - pos);
    pos += exponentLength;
    
    uint8_t specifierLength = lexNumberSpecifier (string + pos, length - pos);
    pos += specifierLength;

    if (pos != length)
    {   token->tokenType = TOKEN_TYPE_INVALID; }

    printf ("Parsed number `%.*s`[%u] got %u:%u:%u:%u\n", length, string, length,
            headLength, tailLength, exponentLength, specifierLength);

    return pos;
}

uint8_t lexNoun (Token* token)
{
    uint8_t length = token->length;
    CharClass class = charClassifier (token->string[0]);
    if (class == CHAR_FLAG_UPPER)
    {
        token->tokenType = length == 1
            ? TOKEN_TYPE_GENERIC_TYPE
            : TOKEN_TYPE_TYPE;
        return length;
    }

    for (size_t i = 0; i < KEYWORD_COUNT; ++i)
    {
        bool match = nstringMatchCstring (length, token->string, keywords[i]);
        if (match)
        {
            token->tokenType = TOKEN_TYPE_KEYWORD;
            token->ident = i;
            return length;
        }
    }

    token->tokenType = TOKEN_TYPE_NOUN;
    return length;
}

void lexer (TokenList* list)
{
    for (size_t i = 0; i < list->count; ++i)
    {
        Token* token = list->data + i;
        switch (token->tokenType)
        {
            case TOKEN_TYPE_NOUN:
                lexNoun (token);
                break;

            case TOKEN_TYPE_LITERAL_NUMBER:
                lexLiteralNumber (token);
                break;
        }
    }
}
