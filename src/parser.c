#include "parser.h"
#include "token.h"
#include <operator.h>

//TODO: Revisit this approach
#if 0

static Token* getPreviousToken (TokenList* tokens, Size i)
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
static Size parseRecursive (TokenList* tokens, Size start, Size end, Size leftTokenIndex, OperatorPrecedence precedence, OperatorContext context)
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

LISTIFY_H(Size);
LISTIFY_C(Size);

Size getLeftTokenIndex (TokenList* tokens, Size i)
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

Size getRightTokenIndex (TokenList* tokens, Size i)
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
    SizeList tokensByPrecedence[OPERATOR_PRECEDENCE_COUNT];
    for (OperatorPrecedence i = 0; i < OPERATOR_PRECEDENCE_COUNT; ++i)
    {
        tokensByPrecedence[i] = createSizeList (64);
    }
    
    OperatorContext context = OPERATOR_CONTEXT_EXPRESSION;
    for (Size i = 0; i < tokens->count; ++i)
    {
        //TODO: something about tracking the current context
        Token* token = &tokens->data[i];
        if (token->type == TOKEN_TYPE_OPERATOR)
        {
            OperatorPrecedence precedence = operatorPrecedences[token->ident];
            appendSizeList (&tokensByPrecedence[precedence], i);
        }
    }

    for (Size precedence = 0; precedence < OPERATOR_PRECEDENCE_COUNT; ++precedence)
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

