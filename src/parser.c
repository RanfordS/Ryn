#include "parser.h"
#include "token.h"
#include <operator.h>

//TODO: Revisit this approach
#if 0

static Token* getPreviousToken (TokenList* tokens, size_t i)
{
    while (0 < --i)
    {
        if (tokens->data[i].type != TOKEN_TYPE_COMMENT)
        {
            return &tokens->data[i];
        }
    }
    return NULL;
}

// We pass around `count` and `tokens` instead of a `TokenList` so we can take
// sub-ranges with ease.
// Return value is the token skip
static size_t parseRecursive (TokenList* tokens, size_t start, size_t end, size_t leftTokenIndex, OperatorPrecedence precedence, OperatorContext context)
{
    // should never result in out-of-bounds as there will always be an end token
    while (tokens->data[start].type == TOKEN_TYPE_COMMENT)
    {
        ++start;
    }
    if (end <= start) { return 0; }

    Token* leftToken = &tokens->data[leftTokenIndex];
    Token* currentToken = &tokens->data[start];

    // if this is an operator, coerce it to be correct
    if (currentToken->type == TOKEN_TYPE_OPERATOR)
    {
        bool forcePrefix = false;

        if (leftTokenIndex == 0)
        {
            forcePrefix = true;
        }
        else
        {
            Token* previousToken = getPreviousToken (tokens, start);
            if (previousToken && previousToken->type == TOKEN_TYPE_OPERATOR)
            {
                OperatorPosition previousPosition
                    = operatorPositions[previousToken->ident];
                //OperatorPosition currentPosition
                //    = operatorPositions[currentToken->ident];
                if (previousPosition == OPERATOR_POSITION_INFIX)
                {
                    forcePrefix = true;
                }
            }
        }

        Operator newOp;
        if (forcePrefix)
        {
            if (!operatorMatchPositionAndContext (currentToken->ident,
                OPERATOR_POSITION_PREFIX, context, &newOp))
            {
                return ERROR_PARSER_OPERATOR_INVALID_CONTEXT;
            }
        }
        else
        {
            if (!operatorMatchContext (currentToken->ident,
                    context, &newOp))
            {
                return ERROR_PARSER_OPERATOR_INVALID_CONTEXT;
            }
        }
        currentToken->ident = newOp;
    }

    bool leftExpectsOperand = false;
    if (leftToken->type == TOKEN_TYPE_OPERATOR)
    {
        OperatorPosition position = operatorPositions[leftToken->ident];
        if (position != OPERATOR_POSITION_SUFFIX)
        {
            leftExpectsOperand = true;
        }
    }

    //TODO: Continue
    //TODO: Be cautious of comments!
}

RynError parse (TokenList* tokens)
{
    //TODO: confirm it's just that simple
    parseRecursive (tokens, 1, tokens->count, 0, 0, OPERATOR_CONTEXT_EXPRESSION);
    return SUCCESS;
}

#else

#include "listify.h"

LISTIFY_H(size_t);
LISTIFY_C(size_t);

size_t getLeftTokenIndex (TokenList* tokens, size_t i)
{
    while (0 < --i)
    {
        if (tokens->data[i].type != TOKEN_TYPE_COMMENT
        &&  tokens->data[i].parsed == false)
        {
            return i;
        }
    }
    return 0;
}

size_t getRightTokenIndex (TokenList* tokens, size_t i)
{
    while (++i < tokens->count)
    {
        if (tokens->data[i].type != TOKEN_TYPE_COMMENT
        &&  tokens->data[i].parsed == false)
        {
            return i;
        }
    }
    return 0;
}

RynError parse (TokenList* tokens)
{
    size_tList tokensByPrecedence[OPERATOR_PRECEDENCE_COUNT];
    for (OperatorPrecedence i = 0; i < OPERATOR_PRECEDENCE_COUNT; ++i)
    {
        tokensByPrecedence[i] = createsize_tList (64);
    }
    
    OperatorContext context = OPERATOR_CONTEXT_EXPRESSION;
    for (size_t i = 0; i < tokens->count; ++i)
    {
        //TODO: something about tracking the current context
        Token* token = &tokens->data[i];
        if (token->type == TOKEN_TYPE_OPERATOR)
        {
            OperatorPrecedence precedence = operatorPrecedences[token->ident];
            appendsize_tList (&tokensByPrecedence[precedence], i);
        }
    }

    for (size_t precedence = 0; precedence < OPERATOR_PRECEDENCE_COUNT; ++precedence)
    {
        if (precedence == OPERATOR_PRECEDENCE_ARGUMENT)
        {
        }
        else
        {
        }
    }
}

#endif

