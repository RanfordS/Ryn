//TODO: Make comments tag on to the token that follows
#include "token.h"
#include "tokenizer.h"
#include "char.h"
#include "operator.h"
#include "stringlib.h"
#include "token.h"
#include "keyword.h"

/*
__ _0 _1 _2 _3 _4 _5 _6 _7 _8 _9 _A _B _C _D _E _F

0_ Nu SH ST ET EO EQ AK BL BS HT LF VT PB CR SO SI
   Ct Ct Ct Ct Ct Ct Ct Ct Ct WS WS Ct Ct WS Ct Ct

1_ DL DC DC DC DC NK SY EB CN EM SB ES FS GS RS US
   Ct Ct Ct Ct Ct Ct Ct Ct Ct Ct Ct Ct Ct Ct Ct Ct

2_    !  "  #  $  %  &  '  (  )  *  +  ,  -  .  / 
   WS Op St Op ?? Op Op Ch Op Op Op Op Op Op Op Op

3_ 0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ? 
   Nu Nu Nu Nu Nu Nu Nu Nu Nu Nu Op Op Op Op Op Op

4_ @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O 
   Op Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt

5_ P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _ 
   Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Op Op Op Op Lt

6_ `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o 
   Op Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt

7_ p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~  DE
   Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Op Op Op Op Ct

Where:
Ct = Control
WS = White Space
Op = Operator
Nu = Number
Lt = Letter
St = String
Ch = Character
 */





int main (int argc, char** argv)
{
    printf ("Starting Ryn complier.\n");
    if (argc != 2)
    {
        printf ("Number of arguments (%i) did not match the expected count\n", argc);
        return -1;
    }
    printf ("File = `%s`\n", argv[1]);

    size_t dataLength;
    char* data = readFile (argv[1], &dataLength);
    //char* data = readFile ("test/HelloWorld.ryn", &dataLength);
    //char* data = readFile ("test/Keys.ryn", &dataLength);
    if (!data) { return -1; }

#if 1
    Tokenizer tokenizer = {
        .sourceData = data,
        .sourceLength = dataLength,
    };

    TokenList tokenList = tokenListCreate (256);

    tokenize (&tokenizer, &tokenList);
#else
    //uint8_t currentTokenType = TOKEN_TYPE_UNKNOWN;
    bool escape = false;

    TokenList tokenList = tokenListCreate (1024);
    Token currentToken = {};
    size_t iStart = 0;

    size_t line = 1;
    size_t col = 0;
    size_t commentDepth = 0;

    for (size_t i = 0; i < dataLength; ++i)
    {
#define TOKEN_START(tc) \
        currentToken.tokenType = tc;\
        currentToken.str = data + i;\
        currentToken.line = line;\
        currentToken.col = col;\
        iStart = i;\

#define TOKEN_PUSH \
        tokenListAppend (&tokenList, currentToken);\
        currentToken = (Token){};\

#define TOKEN_END_NOW(inter) \
        currentToken.len = i - iStart;\
        {inter}\
        TOKEN_PUSH;\
        goto rerun;\

#define TOKEN_END_AFTER(inter) \
        currentToken.len = i + 1 - iStart;\
        {inter}\
        TOKEN_PUSH;\

        char c = data[i];
        CharClass class = charClassifier (c);

        if (currentToken.tokenType != TOKEN_TYPE_LITERAL_STRING)
        {
            if (class == CHAR_CLASS_INVALID)
            {
                printf ("Invalid character (0x%X) encountered\n", c);
            }
        }
        //printf ("Valid char `%c` %i\n", c, class);

        if (c == '\n')
        {
            ++line;
            col = 0;
        }
        else
        {
            ++col;
        }

rerun:
        switch (currentToken.tokenType)
        {
            case TOKEN_TYPE_UNKNOWN:
                if (class & CHAR_FLAG_UPPER)
                {
                    TOKEN_START(TOKEN_TYPE_GENERIC_TYPE);
                }
                else if (class & CHAR_FLAG_LETTER)
                {
                    TOKEN_START(TOKEN_TYPE_NOUN);
                }
                else if (class & CHAR_FLAG_NUMBER)
                {
                    if (c == '0')
                    {
                        TOKEN_START(TOKEN_TYPE_LITERAL_ZERO);
                    }
                    else
                    {
                        TOKEN_START(TOKEN_TYPE_LITERAL_NUMBER);
                    }
                }
                else if (c == '"')
                {
                    TOKEN_START(TOKEN_TYPE_LITERAL_STRING);
                }
                else if (c == '\'')
                {
                    TOKEN_START(TOKEN_TYPE_LITERAL_CHAR);
                }
                else if (class & CHAR_FLAG_OPERATOR)
                {
                    TOKEN_START(TOKEN_TYPE_OPERATOR);
                }
                break;

            case TOKEN_TYPE_GENERIC_TYPE:
                if (class & (CHAR_FLAG_LETTER | CHAR_FLAG_NUMBER))
                {
                    // generics are only one letter long, this is more
                    currentToken.tokenType = TOKEN_TYPE_TYPE;
                }
                else
                {
                    TOKEN_END_NOW();
                }
                break;

            case TOKEN_TYPE_NOUN:
            case TOKEN_TYPE_TYPE:
                if (class & (CHAR_FLAG_LETTER | CHAR_FLAG_NUMBER))
                {
                    // As you were, my good sir...
                }
                else
                {
                    TOKEN_END_NOW(
                    for (size_t j = 0; j < KEYWORD_COUNT; ++j)
                    {
                        if (nstringMatchCstring (currentToken.len, currentToken.str, keywords[j]))
                        {
                            currentToken.id = j;
                            currentToken.tokenType = TOKEN_TYPE_KEYWORD;
                            break;
                        }
                    }
                    );
                }
                break;

            case TOKEN_TYPE_OPERATOR:
                if (class & CHAR_FLAG_OPERATOR)
                {
                    size_t previewLen = i + 1 - iStart;
                    bool found = false;
                    uint16_t operator;
                    for (size_t j = 0; j < OPERATOR_COUNT; ++j)
                    {
                        if (nstringMatchCstring (previewLen, currentToken.str, operators[j]))
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
                            ++commentDepth;
                            currentToken.tokenType = TOKEN_TYPE_COMMENT;
                        }
                        // Do nothing
                    }
                    else
                    {
                        TOKEN_END_NOW();
                    }
                }
                else
                {
                    TOKEN_END_NOW();
                }
                break;

            case TOKEN_TYPE_COMMENT:
                switch (c)
                {
                    case '*':
                        if (data[i - 1] == '(')
                        {
                            ++commentDepth;
                        }
                        break;

                    case ')':
                        if ((data[i - 1] == '*')
                        &&  (data[i - 2] != '('))
                        {
                            if (!--commentDepth)
                            {
                                TOKEN_END_AFTER();
                            }
                        }
                        break;

                    default:
                        break;
                }
                break;

            case TOKEN_TYPE_LITERAL_STRING:
                if (escape)
                {
                    escape = false;
                }
                else
                {
                    if (c == '\\')
                    {
                        escape = true;
                    }
                    else if (c == '"')
                    {
                        TOKEN_END_AFTER();
                    }
                }
                break;

            case TOKEN_TYPE_LITERAL_CHAR:
                if (escape)
                {
                    escape = false;
                    TOKEN_END_AFTER();
                }
                else
                {
                    if (c == '\\')
                    {
                        escape = true;
                    }
                    else if (c == '\'')
                    {
                        TOKEN_END_AFTER();
                    }
                }
                break;

            case TOKEN_TYPE_LITERAL_ZERO:
                if (class & CHAR_FLAG_LETTER)
                {
                    switch (c)
                    {
                        case 'b':
                            currentToken.tokenType = TOKEN_TYPE_LITERAL_BINARY;
                            break;

                        case 'o':
                            currentToken.tokenType = TOKEN_TYPE_LITERAL_OCTAL;
                            break;

                        case 'x':
                            currentToken.tokenType = TOKEN_TYPE_LITERAL_HEXADECIMAL;
                            break;

                        case 'u':
                            currentToken.tokenType = TOKEN_TYPE_LITERAL_UNSIGNED;
                            break;

                        case 'i':
                            currentToken.tokenType = TOKEN_TYPE_LITERAL_INTEGER;
                            break;

                        case 'f':
                            currentToken.tokenType = TOKEN_TYPE_LITERAL_FLOAT;
                            break;

                        default:
                            printf ("Literal Zero: Invalid letter in number at line %llu col %llu\n", line, col);
                            TOKEN_END_NOW();
                            break;
                    }
                }
                else if (class & CHAR_FLAG_NUMBER)
                {
                    printf ("Literal Zero: Zero followed by another number at line %llu col %llu\n", line, col);
                    TOKEN_END_NOW();
                }
                else if (c == '.')
                {
                    currentToken.tokenType = TOKEN_TYPE_LITERAL_DECIMAL;
                }
                else
                {
                    TOKEN_END_NOW();
                }
                break;

            case TOKEN_TYPE_LITERAL_NUMBER:
                if (class & CHAR_FLAG_NUMBER)
                {
                    // As you were, my good sir...
                }
                else if (class & CHAR_FLAG_LETTER)
                {
                    switch (c)
                    {

                        case 'u':
                            currentToken.tokenType = TOKEN_TYPE_LITERAL_UNSIGNED;
                            break;

                        case 'i':
                            currentToken.tokenType = TOKEN_TYPE_LITERAL_INTEGER;
                            break;

                        case 'f':
                            currentToken.tokenType = TOKEN_TYPE_LITERAL_FLOAT;
                            break;

                        case 'e':
                            currentToken.tokenType = TOKEN_TYPE_LITERAL_EXPONENTIAL_FLOAT;
                            break;

                        default:
                            printf ("Literal Number: Invalid character in number at line %llu col %llu\n", line, col);
                            TOKEN_END_NOW();
                            break;
                    }
                }
                else if (c == '.')
                {
                    currentToken.tokenType = TOKEN_TYPE_LITERAL_DECIMAL;
                }
                else
                {
                    TOKEN_END_NOW();
                }
                break;

            case TOKEN_TYPE_LITERAL_DECIMAL:
                if (class & CHAR_FLAG_NUMBER)
                {
                    // As you were, my good sir...
                }
                else
                {
                    if (data[i - 1] == '.')
                    {
                        printf ("Literal Decimal: Invalid character after decimal point in number at line %llu col %llu\n", line, col);
                        TOKEN_END_NOW();
                    }
                    else if (class & CHAR_FLAG_LETTER)
                    {
                        switch (c)
                        {
                            case 'f':
                                currentToken.tokenType = TOKEN_TYPE_LITERAL_FLOAT;
                                break;

                            case 'e':
                                currentToken.tokenType = TOKEN_TYPE_LITERAL_EXPONENTIAL_FLOAT;
                                break;

                            default:
                                printf ("Literal Decimal: Invalid specifier in number at line %llu col %llu\n", line, col);
                                TOKEN_END_NOW();
                                break;
                        }
                    }
                    else
                    {
                        TOKEN_END_NOW();
                    }
                }
                break;

            case TOKEN_TYPE_LITERAL_EXPONENTIAL_FLOAT:
                if (class & CHAR_FLAG_NUMBER)
                {
                    // As you were, my good sir...
                    //TODO: There should probably be some more complex thing to prevent 0 in the first position/
                }
                else if (c == '-')
                {
                    if (data[i-1] != 'e')
                    {
                        printf ("Literal Exponential: Did not expect `-` at line %llu col %llu\n", line, col);
                    }
                }
                else
                {
                    //TODO: This is probably wrong
                    if ((data[i-1] == 'e')
                    ||  (data[i-1] == '-'))
                    {
                        printf ("Literal Exponential: No exponent give at line %llu col %llu\n", line, col);
                    }
                    TOKEN_END_NOW();
                }
                break;

            case TOKEN_TYPE_LITERAL_INTEGER:
            case TOKEN_TYPE_LITERAL_UNSIGNED:
            case TOKEN_TYPE_LITERAL_BINARY_INTEGER:
            case TOKEN_TYPE_LITERAL_BINARY_UNSIGNED:
            case TOKEN_TYPE_LITERAL_OCTAL_INTEGER:
            case TOKEN_TYPE_LITERAL_OCTAL_UNSIGNED:
            case TOKEN_TYPE_LITERAL_HEXADECIMAL_INTEGER:
            case TOKEN_TYPE_LITERAL_HEXADECIMAL_UNSIGNED:
                if (class & CHAR_FLAG_NUMBER)
                {
                    if (c == '0')
                    {
                        printf ("Literal Int or UInt: Invalid zero at the start of type size specification\n");
                        TOKEN_END_NOW();
                    }
                    else
                    {
                        ++currentToken.tokenType;
                    }
                }
                else if (class & CHAR_FLAG_LETTER)
                {
                    printf ("Literal Int or UInt: Invalid character in number at line %llu col %llu\n", line, col);
                    TOKEN_END_NOW();
                }
                break;

            case TOKEN_TYPE_LITERAL_INTEGER_SPECIFIED:
            case TOKEN_TYPE_LITERAL_UNSIGNED_SPECIFIED:
            case TOKEN_TYPE_LITERAL_BINARY_INTEGER_SPECIFIED:
            case TOKEN_TYPE_LITERAL_BINARY_UNSIGNED_SPECIFIED:
            case TOKEN_TYPE_LITERAL_OCTAL_INTEGER_SPECIFIED:
            case TOKEN_TYPE_LITERAL_OCTAL_UNSIGNED_SPECIFIED:
            case TOKEN_TYPE_LITERAL_HEXADECIMAL_INTEGER_SPECIFIED:
            case TOKEN_TYPE_LITERAL_HEXADECIMAL_UNSIGNED_SPECIFIED:
            case TOKEN_TYPE_LITERAL_FLOAT_SPECIFIED:
            case TOKEN_TYPE_LITERAL_EXPONENTIAL_FLOAT_SPECIFIED:
                if (class & CHAR_FLAG_NUMBER)
                {
                    // As you were, my good sir...
                }
                else
                {
                    if (class & CHAR_FLAG_LETTER)
                    {
                        printf ("Literal Specified: Invalid character in number at line %llu col %llu\n", line, col);
                    }
                    TOKEN_END_NOW();
                }
                break;

            default:
                printf ("Could not handle input, unhandled token type\n");
                TOKEN_END_NOW();
                break;
        }
        //printf ("Token type: %s\n", tokenType[currentToken.tokenType]);
    }
#endif

    printf ("\nList of Tokens\n");
    for (size_t t = 0; t < tokenList.count; ++t)
    {
        Token token = tokenList.list[t];
        printf ("0x%04llX[%03llu,%02llu] - %s `%.*s`\n", t, token.line, token.col, tokenType[token.tokenType], token.len, token.str);
    }

    free (data);

    return 0;
}

