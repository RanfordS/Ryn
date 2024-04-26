#include "tokenizer.h"
#include "char.h"
#include "keyword.h"
#include "stringlib.h"
#include "operator.h"

static void bracketPush (Tokenizer* tokenizer, TokenList* list)
{
    size_t i = tokenizer->bracketDepth++;
    if (tokenizer->bracketStackSize < tokenizer->bracketDepth)
    {
        size_t size = tokenizer->bracketStackSize *= 2;
        tokenizer->bracketStackType =
            realloc (tokenizer->bracketStackType,
                size*sizeof (*tokenizer->bracketStackType));
        tokenizer->bracketIndex =
            realloc (tokenizer->bracketIndex,
                size*sizeof (*tokenizer->bracketIndex));
    }
    tokenizer->bracketStackType[i] = tokenizer->currentToken.id;
    tokenizer->bracketIndex[i] = list->count;
}

static TokenizerCode bracketPop (Tokenizer* tokenizer, TokenList* list)
{
    if (tokenizer->bracketDepth == 0)
    {
        return TOKENIZER_ERROR_TOO_MANY_CLOSING_BRACKETS;
    }

    size_t lhsListIndex = tokenizer->bracketIndex[--tokenizer->bracketDepth];
    size_t rhsListIndex = list->count;
    Token* lhsToken = list->list + lhsListIndex;
    Token* rhsToken = &tokenizer->currentToken;

    lhsToken->leftIndex = rhsListIndex;
    rhsToken->leftIndex = lhsListIndex;

    Operator lhs = lhsToken->id;
    Operator rhs = rhsToken->id;

    switch (lhs)
    {
        case OPERATOR_PAREN_OPEN:
        case OPERATOR_MACRO_OPEN:
            return rhs == OPERATOR_PAREN_CLOSE
                ? TOKENIZER_CONTINUE
                : TOKENIZER_ERROR_MISMATCHED_BRACKET;

        case OPERATOR_ARRAY_INDEX_OPEN:
        case OPERATOR_BIT_INDEX_OPEN:
            return rhs == OPERATOR_INDEX_CLOSE
                ? TOKENIZER_CONTINUE
                : TOKENIZER_ERROR_MISMATCHED_BRACKET;

        case OPERATOR_BLOCK_OPEN:
            return rhs == OPERATOR_BLOCK_CLOSE
                ? TOKENIZER_CONTINUE
                : TOKENIZER_ERROR_MISMATCHED_BRACKET;
    }

    return TOKENIZER_ERROR_UNHANDLED_BRACKET_TYPE;
}


static void tokenStart (Tokenizer* tokenizer, size_t i, TokenType type)
{
    tokenizer->currentToken.tokenType = type;
    tokenizer->currentToken.str   = tokenizer->sourceData + i;
    tokenizer->currentToken.line  = tokenizer->line;
    tokenizer->currentToken.col   = tokenizer->col;
    tokenizer->currentToken.index = i;
}

static TokenizerCode tokenPush (Tokenizer* tokenizer, TokenList* list)
{
    Token* token = &tokenizer->currentToken;
    switch (token->tokenType)
    {
        case TOKEN_TYPE_NOUN:
            for (size_t i = 0; i < KEYWORD_COUNT; ++i)
            {
                if (nstringMatchCstring (
                        token->len, token->str,
                        keywords[i]))
                {
                    token->id = i;
                    token->tokenType = TOKEN_TYPE_KEYWORD;
                }
            }
            break;

        case TOKEN_TYPE_OPERATOR:
            OperatorFlags flags = operatorFlags[token->id];
            if (flags & BRACKET_OPEN)
            {
                bracketPush (tokenizer, list);
            }
            if (flags & BRACKET_CLOSE)
            {
                TokenizerCode error = bracketPop (tokenizer, list);
                if (error)
                {
                    return error;
                }
            }
            break;
    }
    tokenListAppend (list, *token);
    tokenizer->currentToken = (Token){};
    return TOKENIZER_CONTINUE;
}



#define TOKENIZER_INITIAL_BRACKET_STACK_SIZE 32

Tokenizer tokenizerCreate (char* path, size_t length, char* data)
{
    const size_t bSize = TOKENIZER_INITIAL_BRACKET_STACK_SIZE;
    Tokenizer new =
    {
        .sourcePath = path,
        .sourceData = data,
        .sourceLength = length,
        .bracketStackSize = bSize,
        .bracketIndex = malloc (bSize*sizeof (new.bracketIndex)),
        .bracketStackType = malloc (bSize*sizeof (new.bracketStackType)),
    };
    return new;
}



#define TOKENIZER_STATE(name) \
    TokenizerCode name (\
        Tokenizer* tokenizer,\
        size_t i,\
        char c,\
        CharClass class)



static TOKENIZER_STATE(unknown)
{
    TokenType newType = TOKEN_TYPE_UNKNOWN;

    if (class & CHAR_FLAG_UPPER)
    {
        newType = TOKEN_TYPE_GENERIC_TYPE;
    }
    else if (class & CHAR_FLAG_LETTER)
    {
        newType = TOKEN_TYPE_NOUN;
    }
    else if (class & CHAR_FLAG_NUMBER)
    {
        if (c == '0')
        {
            newType = TOKEN_TYPE_LITERAL_ZERO;
        }
        else
        {
            newType = TOKEN_TYPE_LITERAL_NUMBER;
        }
    }
    else if (c == '"')
    {
        newType = TOKEN_TYPE_LITERAL_STRING;
    }
    else if (c == '\'')
    {
        newType = TOKEN_TYPE_LITERAL_CHAR;
    }
    else if (class & CHAR_FLAG_OPERATOR)
    {
        newType = TOKEN_TYPE_OPERATOR;
    }

    // -*- //

    if (newType != TOKEN_TYPE_UNKNOWN)
    {
        tokenStart (tokenizer, i, newType);
    }
    return TOKENIZER_CONTINUE;
}



static TOKENIZER_STATE(generic)
{
    (void)i; (void)c;
    if (class & (CHAR_FLAG_LETTER | CHAR_FLAG_NUMBER))
    {
        // generics are only one letter long, this is more
        tokenizer->currentToken.tokenType = TOKEN_TYPE_TYPE;
        return TOKENIZER_CONTINUE;
    }
    return TOKENIZER_END_TOKEN_NOW;
}



static TOKENIZER_STATE(noun)
{
    if (class & (CHAR_FLAG_LETTER | CHAR_FLAG_NUMBER))
    {
        return TOKENIZER_CONTINUE;
    }
    return TOKENIZER_END_TOKEN_NOW;
}



static TOKENIZER_STATE(operator)
{
    if (class & CHAR_FLAG_OPERATOR)
    {
        size_t previewLen = i + 1 - tokenizer->currentToken.index;
        bool found = false;
        Operator operator;
        for (size_t j = tokenizer->currentToken.id; j < OPERATOR_COUNT; ++j)
        {
            if (nstringMatchCstring (
                    previewLen, tokenizer->currentToken.str,
                    operators[j]))
            {
                found = true;
                operator = j;
                break;
            }
        }
        if (found)
        {
            if (operator == OPERATOR_COMMENT_OPEN)
            {
                tokenizer->commentDepth = 1;
                tokenizer->currentToken.tokenType = TOKEN_TYPE_COMMENT;
            }
            tokenizer->currentToken.id = operator;
            return TOKENIZER_CONTINUE;
        }
    }
    return TOKENIZER_END_TOKEN_NOW;
}



static TOKENIZER_STATE(comment)
{
    char* data = tokenizer->sourceData;
    switch (c)
    {
        case '*':
            if (data[i - 1] == '(')
            {
                ++tokenizer->commentDepth;
            }
            return TOKENIZER_CONTINUE;

        case '(': // Included this explicitly to get matching braces.
        default:
            return TOKENIZER_CONTINUE;

        case ')':
            if (data[i - 1] == '*'
            &&  data[i - 2] != '(') // )
            {
                if (!--tokenizer->commentDepth)
                {
                    return TOKENIZER_END_TOKEN_AFTER;
                }
            }
            return TOKENIZER_CONTINUE;
    }
}



static TOKENIZER_STATE(string)
{
    if (tokenizer->escape)
    {
        tokenizer->escape = false;
    }
    else
    {
        if (c == '\\')
        {
            tokenizer->escape = true;
        }
        else if (c == '"')
        {
            return TOKENIZER_END_TOKEN_AFTER;
        }
    }
    return TOKENIZER_CONTINUE;
}



static TOKENIZER_STATE(character)
{
    if (tokenizer->escape)
    {
        tokenizer->escape = false;
        return TOKENIZER_END_TOKEN_AFTER;
    }
    else
    {
        if (c == '\\')
        {
            tokenizer->escape = true;
        }
        else
        {
            return TOKENIZER_END_TOKEN_AFTER;
        }
    }
    return TOKENIZER_CONTINUE;
}

    

static TOKENIZER_STATE(zero)
{
    if (class & CHAR_FLAG_LETTER)
    {
        TokenType newType;
        switch (c)
        {
            case 'b':
                newType = TOKEN_TYPE_LITERAL_BINARY;
                break;

            case 'o':
                newType = TOKEN_TYPE_LITERAL_OCTAL;
                break;

            case 'x':
                newType = TOKEN_TYPE_LITERAL_HEXADECIMAL;
                break;

            case 'u':
                newType = TOKEN_TYPE_LITERAL_UNSIGNED;
                break;

            case 'i':
                newType = TOKEN_TYPE_LITERAL_INTEGER;
                break;

            case 'f':
                newType = TOKEN_TYPE_LITERAL_FLOAT;
                break;

            default:
                //TODO: Handle error condition of invalid letter in number.
                return TOKENIZER_END_TOKEN_NOW;
        }

        tokenizer->currentToken.tokenType = newType;
        return TOKENIZER_CONTINUE;
    }
    else if (class & CHAR_FLAG_NUMBER)
    {
        //TODO: Handle error condition of zero followed by another number.
        return TOKENIZER_END_TOKEN_NOW;
    }
    else if (c == '.')
    {
        tokenizer->currentToken.tokenType = TOKEN_TYPE_LITERAL_DECIMAL;
    }
    return TOKENIZER_END_TOKEN_NOW;
}

    

static TOKENIZER_STATE(number)
{
    if (class & CHAR_FLAG_NUMBER)
    {
        return TOKENIZER_CONTINUE;
    }

    if (class & CHAR_FLAG_LETTER)
    {
        TokenType newType;
        switch (c)
        {
            case 'u':
                newType = TOKEN_TYPE_LITERAL_UNSIGNED;
                break;

            case 'i':
                newType = TOKEN_TYPE_LITERAL_INTEGER;
                break;

            case 'f':
                newType = TOKEN_TYPE_LITERAL_FLOAT;
                break;

            case 'd':
                newType = TOKEN_TYPE_LITERAL_FLOAT;
                break;

            case 'e':
                newType = TOKEN_TYPE_LITERAL_EXPONENTIAL_FLOAT;
                break;

            default:
                //TODO: Handle error condition of invalid specifier.
                return TOKENIZER_END_TOKEN_NOW;
        }

        tokenizer->currentToken.tokenType = newType;
        return TOKENIZER_CONTINUE;
    }

    if (c == '.')
    {
        tokenizer->currentToken.tokenType = TOKEN_TYPE_LITERAL_DECIMAL;
        return TOKENIZER_CONTINUE;
    }
    return TOKENIZER_END_TOKEN_NOW;
}



static TOKENIZER_STATE(decimal)
{
    if (class & CHAR_FLAG_NUMBER)
    {
        return TOKENIZER_CONTINUE;
    }

    if (tokenizer->sourceData[i - 1] == '.')
    {
        //TODO: Handle error of invalid character after decimal point.
        return TOKENIZER_END_TOKEN_NOW;
    }

    if (class & CHAR_FLAG_LETTER)
    {
        TokenType newType;
        switch (c)
        {
            case 'f':
                newType = TOKEN_TYPE_LITERAL_FLOAT;
                break;

            case 'e':
                newType = TOKEN_TYPE_LITERAL_EXPONENTIAL;
                break;

            default:
                //TODO: Handle error of invalid specifier.
                return TOKENIZER_END_TOKEN_NOW;
        }

        tokenizer->currentToken.tokenType = newType;
    }
    return TOKENIZER_END_TOKEN_NOW;
}



static TOKENIZER_STATE(exponential)
{
    if (class & CHAR_FLAG_NUMBER)
    {
        //TODO: Consider adding logic around zero.
        return TOKENIZER_CONTINUE;
    }

    char prev = tokenizer->sourceData[i-1];
    bool prevIsNumber = charClassifier (prev) & CHAR_FLAG_NUMBER;

    switch (c)
    {
        case 'f':
        case 'd':
            if (!prevIsNumber)
            {
                //TODO: Handle error of exponent having no numbers.
                return TOKENIZER_END_TOKEN_NOW;
            }
            tokenizer->currentToken.tokenType = c == 'f' ?
                TOKEN_TYPE_LITERAL_EXPONENTIAL_FLOAT :
                TOKEN_TYPE_LITERAL_EXPONENTIAL_DOUBLE;
            return TOKENIZER_END_TOKEN_AFTER;

        case '-':
        case '+':
            if (prev == 'e')
            {
                return TOKENIZER_CONTINUE;
            }
            return TOKENIZER_END_TOKEN_NOW;
    }

    if (class & CHAR_FLAG_LETTER)
    {
        //TODO: Handle error of unexpected letter in number.
        return TOKENIZER_END_TOKEN_NOW;
    }

    if (charClassifier (prev) & CHAR_FLAG_NUMBER)
    {
        return TOKENIZER_END_TOKEN_NOW;
    }
    //TODO: Handle error of exponent having no numbers.
    return TOKENIZER_END_TOKEN_NOW;
}

static TOKENIZER_STATE(typeGiven)
{
    if (class & CHAR_FLAG_NUMBER)
    {
        if (c == '0')
        {
            //TODO: Handle error of type size starting with 0.
            return TOKENIZER_END_TOKEN_NOW;
        }
        ++tokenizer->currentToken.tokenType;
        return TOKENIZER_CONTINUE;
    }
    
    if (class & CHAR_FLAG_LETTER)
    {
        //TODO: Handle error of letter in type size.
        return TOKENIZER_END_TOKEN_NOW;
    }

    return TOKENIZER_END_TOKEN_NOW;
}



static TOKENIZER_STATE(sizeGiven)
{
    if (class & CHAR_FLAG_NUMBER)
    {
        return TOKENIZER_CONTINUE;
    }

    if (class & CHAR_FLAG_LETTER)
    {
        //TODO: Handle error of letter in type size.
        return TOKENIZER_END_TOKEN_NOW;
    }
    return TOKENIZER_END_TOKEN_NOW;
}



static TOKENIZER_STATE((*handlers[TOKEN_TYPE_COUNT])) =
{
    [TOKEN_TYPE_UNKNOWN] = unknown,
    [TOKEN_TYPE_GENERIC_TYPE] = generic,
    [TOKEN_TYPE_NOUN] = noun,
    [TOKEN_TYPE_TYPE] = noun,
    [TOKEN_TYPE_OPERATOR] = operator,
    [TOKEN_TYPE_COMMENT] = comment,
    [TOKEN_TYPE_LITERAL_STRING] = string,
    [TOKEN_TYPE_LITERAL_CHAR] = character,
    [TOKEN_TYPE_LITERAL_ZERO] = zero,
    [TOKEN_TYPE_LITERAL_NUMBER] = number,
    [TOKEN_TYPE_LITERAL_DECIMAL] = decimal,
    [TOKEN_TYPE_LITERAL_EXPONENTIAL] = exponential,
    [TOKEN_TYPE_LITERAL_INTEGER] = typeGiven,
    [TOKEN_TYPE_LITERAL_UNSIGNED] = typeGiven,
    [TOKEN_TYPE_LITERAL_BINARY_INTEGER] = typeGiven,
    [TOKEN_TYPE_LITERAL_BINARY_UNSIGNED] = typeGiven,
    [TOKEN_TYPE_LITERAL_OCTAL_INTEGER] = typeGiven,
    [TOKEN_TYPE_LITERAL_OCTAL_UNSIGNED] = typeGiven,
    [TOKEN_TYPE_LITERAL_HEXADECIMAL_INTEGER] = typeGiven,
    [TOKEN_TYPE_LITERAL_HEXADECIMAL_UNSIGNED] = typeGiven,
    [TOKEN_TYPE_LITERAL_INTEGER_SPECIFIED] = sizeGiven,
    [TOKEN_TYPE_LITERAL_UNSIGNED_SPECIFIED] = sizeGiven,
    [TOKEN_TYPE_LITERAL_BINARY_INTEGER_SPECIFIED] = sizeGiven,
    [TOKEN_TYPE_LITERAL_BINARY_UNSIGNED_SPECIFIED] = sizeGiven,
    [TOKEN_TYPE_LITERAL_OCTAL_INTEGER_SPECIFIED] = sizeGiven,
    [TOKEN_TYPE_LITERAL_OCTAL_UNSIGNED_SPECIFIED] = sizeGiven,
    [TOKEN_TYPE_LITERAL_HEXADECIMAL_INTEGER_SPECIFIED] = sizeGiven,
    [TOKEN_TYPE_LITERAL_HEXADECIMAL_UNSIGNED_SPECIFIED] = sizeGiven,
    [TOKEN_TYPE_LITERAL_FLOAT_SPECIFIED] = sizeGiven,
    [TOKEN_TYPE_LITERAL_EXPONENTIAL_FLOAT_SPECIFIED] = sizeGiven,
};

TokenizerCode tokenize (Tokenizer* tokenizer, TokenList* list)
{
    printf ("Tokenizing\n");
    for (size_t i = 0; i < tokenizer->sourceLength; ++i)
    {
        char c = tokenizer->sourceData[i];
        CharClass class = charClassifier (c);
        rerun:
        TokenType type = tokenizer->currentToken.tokenType;

        if (type != TOKEN_TYPE_LITERAL_STRING
        &&  class == CHAR_CLASS_INVALID)
        {
            printf ("Invalid character (0x%X) encountered\n", c);
        }

        if (c == '\n')
        {
            ++tokenizer->line; tokenizer->col = 0;
        }
        else
        {
            ++tokenizer->col;
        }

        TOKENIZER_STATE((*handler)) = handlers[type];
        if (!handler)
        {
            printf ("Unhandled token type %s\n", tokenType[type]);
            exit (-1);
        }
        TokenizerCode result = handler (tokenizer, i, c, class);
        switch (result)
        {
            case TOKENIZER_CONTINUE:
                break;

            case TOKENIZER_END_TOKEN_NOW:
                tokenizer->currentToken.len =
                    i - tokenizer->currentToken.index;
                if (tokenPush (tokenizer, list))
                {
                    return 1;
                }
                goto rerun;

            case TOKENIZER_END_TOKEN_AFTER:
                tokenizer->currentToken.len =
                    i + 1 - tokenizer->currentToken.index;
                if (tokenPush (tokenizer, list))
                {
                    return 1;
                }
                break;

            default:
                return result;
        }
    }
    return false;
}
