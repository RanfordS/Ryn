#include "parser.h"
#include "token.h"

// We pass around `count` and `tokens` instead of a `TokenList` so we can take
// sub-ranges with ease.
// Return value is the token skip
static size_t parseRecursive (TokenList* tokens, size_t start, size_t end, size_t leftTokenIndex, OperatorContext context)
{
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
            Token* previousToken = &tokens->data[start - 1];
            if (previousToken->type == TOKEN_TYPE_OPERATOR)
            {
                OperatorPosition previousPosition
                    = operatorPositions[previousToken->ident];
                OperatorPosition currentPosition
                    = operatorPositions[currentToken->ident];
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

    //TODO: Continue
}

RynError parse (TokenList* tokens)
{
    //TODO: confirm it's just that simple
    parseRecursive (tokens, 1, tokens->count, 0, OPERATOR_CONTEXT_EXPRESSION);
    return SUCCESS;
}

