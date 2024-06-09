#include "core.h"
#include "operator.h"
#include "token.h"

// we pass around `count` and `tokens` instead of a `TokenList` so we can take
// sub-ranges with ease
static size_t parseRecursive (size_t count, Token* tokens, Token* leftTree, OperatorContext context)
{
    if (count == 0)
    {
        //TODO: Error, expected more
    }
    // if we have two identifiers in a row, we start and arg chain
    // alternatively, leftTree could be a suffix or prefix operator

    // if previousPrecedence <= current, the sub-tree is our left operator
    // else we are the sub-tree's right operator

    bool leftExpectingOperand = false;
    if (leftTree->type == TOKEN_TYPE_OPERATOR)
    {
        OperatorPosition position = operatorPositions[leftTree->ident];
        if (position != OPERATOR_POSITION_SUFFIX)
        {
            leftExpectingOperand = true;
        }
    }
}
