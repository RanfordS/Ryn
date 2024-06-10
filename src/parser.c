#include "parser.h"
#include <token.h>

// We pass around `count` and `tokens` instead of a `TokenList` so we can take
// sub-ranges with ease.
// Return value is the token skip
static size_t parseRecursive (TokenList* tokens, size_t start, size_t end, size_t leftTokenIndex, OperatorContext context)
{
    if (start >= end)
    {
        //TODO: Error, expected more
    }

    // if we have two identifiers in a row, we start and arg chain
    // alternatively, leftTree could be a suffix or prefix operator

    // if previousPrecedence <= current, the sub-tree is our left operator
    // else we are the sub-tree's right operator

    Token* leftToken = &tokens->data[leftTokenIndex];
    Token* currentToken = &tokens->data[start];

    if (currentToken->type == TOKEN_TYPE_OPERATOR
    &&  operatorPositions[currentToken->ident] == OPERATOR_POSITION_BRACKET_OPEN)
    {
        size_t closingTokenIndex = currentToken->rightIndex;
        parseRecursive (tokens, start + 1, closingTokenIndex - 1, 0, OPERATOR_CONTEXT_EXPRESSION);
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

    if (currentToken->type == TOKEN_TYPE_OPERATOR)
    {
        Operator operator;
        if (!operatorMatchContext (currentToken->ident, context, &operator))
        {
            currentToken->error = ERROR_PARSER_OPERATOR_INVALID_CONTEXT;
            return 1;
        }
    }
}

RynError parse (TokenList* tokens)
{
    //TODO: confirm it's just that simple
    parseRecursive (tokens, 0, tokens->count, 0, OPERATOR_CONTEXT_EXPRESSION);
    return SUCCESS;
}
