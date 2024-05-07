#include "tokenizer.h"
#include "char.h"
#include "keyword.h"
#include "stringlib.h"
#include "operator.h"




enum
{
    TOKENIZER_STATE_TOKEN_CONTINUE,
    TOKENIZER_STATE_END_TOKEN_NOW,
    TOKENIZER_STATE_END_TOKEN_AFTER,
};



TokenizerError tokenize (size_t length, const char* source, TokenList* list)
{
    Token token =
    {
        .tokenType = TOKEN_TYPE_START_OF_FILE,
    };
    appendTokenList (list, token);

    token = (Token){};

    typeof(token.line)   line   = 1;
    typeof(token.column) column = 0;

    bool escape = false;
    uint8_t commentDepth = 0;

    for (size_t i = 0; i < length; ++i)
    {
        char c = source[i];
        CharClass class = charClassifier (c);

        if (c == '\n') ++line, column = 1;
        else ++column;

        if (token.tokenType != TOKEN_TYPE_COMMENT
        &&  token.tokenType != TOKEN_TYPE_LITERAL_STRING
        &&  class & CHAR_CLASS_INVALID)
        {
            Token error =
            {
                .tokenType = TOKEN_TYPE_INVALID,
                .index = i,
                .line = line,
                .column = column,
            };
            appendTokenList (list, error);
        }

        rerun:
        uint8_t state;
        switch (token.tokenType)
        {
            case TOKEN_TYPE_NONE:
                if (class & CHAR_FLAG_NUMBER)
                {   token.tokenType = TOKEN_TYPE_LITERAL_NUMBER; }
                else
                if (class & CHAR_MASK_NOUN)
                {   token.tokenType = TOKEN_TYPE_NOUN; }
                else
                if (c == '"')
                {   token.tokenType = TOKEN_TYPE_LITERAL_STRING; }
                else
                if (c == '\'')
                {   token.tokenType = TOKEN_TYPE_LITERAL_CHAR; }
                else
                if (class & CHAR_FLAG_OPERATOR)
                {   
                    token.tokenType = TOKEN_TYPE_OPERATOR; 
                    Operator operator;
                    operatorFind (0, 1, source + i, &operator);
                    token.ident = operator;
                }

                if (token.tokenType != TOKEN_TYPE_NONE)
                {
                    token.index  = i;
                    token.string = source + i;
                    token.line   = line;
                    token.column = column;
                }
                state = TOKENIZER_STATE_TOKEN_CONTINUE;
                break;

            case TOKEN_TYPE_LITERAL_NUMBER:
                if (c == '.')
                {
                    state = TOKENIZER_STATE_TOKEN_CONTINUE;
                }
                if ((c == '-' || c == '+')
                &&  source[i-1] == 'e')
                {
                    state = TOKENIZER_STATE_TOKEN_CONTINUE;
                }
                else
                {
                    state = class & CHAR_MASK_NOUN
                        ? TOKENIZER_STATE_TOKEN_CONTINUE
                        : TOKENIZER_STATE_END_TOKEN_NOW;
                }
                break;

            case TOKEN_TYPE_NOUN:
                state = class & CHAR_MASK_NOUN
                    ? TOKENIZER_STATE_TOKEN_CONTINUE
                    : TOKENIZER_STATE_END_TOKEN_NOW;
                break;

            case TOKEN_TYPE_LITERAL_STRING:
                state = TOKENIZER_STATE_TOKEN_CONTINUE;
                if (escape)
                {   escape = false; }
                else if (c == '\\')
                {   escape = true; }
                else if (c == '"')
                {   state = TOKENIZER_STATE_END_TOKEN_AFTER; }
                break;

            case TOKEN_TYPE_LITERAL_CHAR:
                state = TOKENIZER_STATE_TOKEN_CONTINUE;
                if (escape)
                {
                    escape = false;
                    state = TOKENIZER_STATE_END_TOKEN_AFTER;
                }
                else if (c == '\\')
                {   escape = true; }
                else
                {   state = TOKENIZER_STATE_END_TOKEN_AFTER; }
                break;

            case TOKEN_TYPE_OPERATOR:
            {
                size_t preview = i - token.index + 1;
                char* string = source + token.index;
                Operator operator;
                bool found = operatorFind
                    (token.ident, preview, string, &operator);
                
                if (found)
                {
                    if (operator == OPERATOR_COMMENT_OPEN)
                    {
                        commentDepth = 1;
                        operator = 0;
                        token.tokenType = TOKEN_TYPE_COMMENT;
                    }
                    token.ident = operator;
                    state = TOKENIZER_STATE_TOKEN_CONTINUE;
                }
                else
                {   state = TOKENIZER_STATE_END_TOKEN_NOW; }
            }   break;

            case TOKEN_TYPE_COMMENT:
                state = TOKENIZER_STATE_TOKEN_CONTINUE;
                switch (c)
                {
                    case '*':
                        if (source[i - 1] == '(') // )
                        {   ++commentDepth; }
                        break;

                    // (
                    case ')':
                        if (source[i - 1] == '*'
                        &&  source[i - 2] != '(') // )
                        {
                            if (0 == --commentDepth)
                            {   state = TOKENIZER_STATE_END_TOKEN_AFTER; }
                        }
                        break;
                }
                break;
        }

        switch (state)
        {
            case TOKENIZER_STATE_END_TOKEN_NOW:
                token.length = i - token.index;
                appendTokenList (list, token);
                token = (Token){};
                goto rerun;

            case TOKENIZER_STATE_END_TOKEN_AFTER:
                token.length = i - token.index + 1;
                appendTokenList (list, token);
                token = (Token){};
                break;

            default:
                break;
        }
    }

    switch (token.tokenType)
    {
        case TOKEN_TYPE_LITERAL_STRING:
        case TOKEN_TYPE_LITERAL_CHAR:
        case TOKEN_TYPE_COMMENT:
            //TODO: handle error, incomplete token
            break;
    }

    token = (Token)
    {
        .index = length,
        .line = line,
        .column = column,
        .tokenType = TOKEN_TYPE_END_OF_FILE,
    };
    appendTokenList (list, token);

    return TOKENIZER_SUCCESS;
}

