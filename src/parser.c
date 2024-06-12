#include "parser.h"
#include "token.h"
#include "operator.h"

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

//TODO: Add provisions around brackets
//eg `a[b]` on `b` where `[` is marked as used and thus grabs something it shouldn't
Size getLeftTokenIndex (TokenList* tokens, Size i)
{
    printf ("Getting token left of %llu: ", i);
    while (0 < --i)
    {
        if (tokens->data[i].type != TOKEN_TYPE_COMMENT
        &&  tokens->data[i].hasParent == false)
        {
            printf ("found at %llu\n", i);
            return i;
        }
    }
    printf ("none found\n");
    return 0;
}

Size getRightTokenIndex (TokenList* tokens, Size i)
{
    printf ("Getting token left of %llu: ", i);
    while (++i < tokens->count - 1)
    {
        if (tokens->data[i].type != TOKEN_TYPE_COMMENT
        &&  tokens->data[i].hasParent == false)
        {
            printf ("found at %llu\n", i);
            return i;
        }
    }
    printf ("none found\n");
    return 0;
}

// note for all parser functions contained within, return errors can be safely
// propagated with |= as there is only two codes, all other types of errors are
// applied to tokens

//TODO: handle the case of `;` where the right operand is optional
static RynError parserHandleSide (TokenList* tokens, Token* parent, Size childIndex, RynError missingOperandError)
{
    if (!childIndex)
    {
        parent->error = missingOperandError;
        return ERROR_PARSER_HAD_ERROR;
    }

    Token* childToken = &tokens->data[childIndex];
    childToken->hasParent = true;
    if (!parent->error && childToken->error)
    {
        parent->error = ERROR_PARSER_INHERITED;
        return ERROR_PARSER_HAD_ERROR;
    }
    return SUCCESS;
}

static RynError parseOperator (TokenList* tokens, SizeList* tokenIndices)
{
    RynError error = SUCCESS;
    for (Size i = 0; i < tokenIndices->count; ++i)
    {
        Size tokenIndex = tokenIndices->data[i];
        Token* token = &tokens->data[tokenIndex];
        OperatorPosition position = operatorPositions[token->ident];

        if (position != OPERATOR_POSITION_PREFIX)
        {
            Size leftIndex = getLeftTokenIndex (tokens, tokenIndex);
            token->leftIndex = leftIndex;
            error |= parserHandleSide
                (tokens, token, leftIndex, ERROR_PARSER_NO_LHS);
        }
        if (position != OPERATOR_POSITION_SUFFIX)
        {
            Size rightIndex = getRightTokenIndex (tokens, tokenIndex);
            token->rightIndex = rightIndex;
            error |= parserHandleSide
                (tokens, token, rightIndex, ERROR_PARSER_NO_RHS);
        }
    }
    return error;
}

RynError parse (TokenList* tokens)
{
    RynError error = SUCCESS;
    printf ("Running parse\n");
    SizeList tokenIndicesByPrecedence[OPERATOR_PRECEDENCE_COUNT];
    for (OperatorPrecedence i = 0; i < OPERATOR_PRECEDENCE_COUNT; ++i)
    {
        tokenIndicesByPrecedence[i] = createSizeList (64);
    }
    
    OperatorContext context = OPERATOR_CONTEXT_EXPRESSION;
    printf ("Categorising tokens\n");
    for (Size i = 0; i < tokens->count; ++i)
    {
        //TODO: something about tracking the current context
        //eg `Func` encountered, enter arg spec mode until the body
        //TODO: use the collected context to refine the operators
        Token* token = &tokens->data[i];
        if (token->type == TOKEN_TYPE_OPERATOR)
        {
            OperatorPrecedence precedence = operatorPrecedences[token->ident];
            appendSizeList (&tokenIndicesByPrecedence[precedence], i);
            //printf ("Token %llu is an operator of precedence %i\n", i, precedence);
        }
    }

    for (OperatorPrecedence precedence = 0; precedence < OPERATOR_PRECEDENCE_COUNT; ++precedence)
    {
        if (precedence == OPERATOR_PRECEDENCE_ARGUMENT)
        {
            printf ("Evaluating precedence level %i, parsing arguments\n", precedence);
            // arguments are not separated by operators, so we handle them separately
            // guess we getRight until we hit an operator of higher precedence
            bool makePreviousParented = false;
            Size previousIndex;
            for (Size i = 1; i < tokens->count - 1; ++i)
            {
                printf ("\nToken %llu\n", i);
                Token* token = &tokens->data[i];
                if (token->hasParent) { continue; }
                printf ("  had no parent\n");

                if (token->type == TOKEN_TYPE_OPERATOR)
                {
                    printf ("  is an operator\n");
                    OperatorPrecedence p = operatorPrecedences[token->ident];
                    if (p > OPERATOR_PRECEDENCE_ARGUMENT) { continue; };
                    printf ("  but of lower precedence\n");
                }

                Size rightIndex = getRightTokenIndex (tokens, i);
                if (!rightIndex) { continue; }
                printf ("  right index found\n");

                Token* rightToken = &tokens->data[rightIndex];
                if (rightToken->type == TOKEN_TYPE_OPERATOR)
                {
                    printf ("  right index is an operator\n");
                    OperatorPrecedence precedence =
                        operatorPrecedences[rightToken->ident];
                    if (precedence > OPERATOR_PRECEDENCE_ARGUMENT) { continue; }
                    printf ("  but it is of lower precedence\n");
                }
                printf ("Thus it is our next token\n");

                if (makePreviousParented)
                {
                    makePreviousParented = false;
                    tokens->data[previousIndex].hasParent = true;
                }

                token->nextIndex = rightIndex;
                makePreviousParented = true;
                previousIndex = rightIndex;
                //rightToken->hasParent = true;
                // sub 1 so that we loop back to this
                i = rightIndex - 1;
            }
            if (makePreviousParented)
            {
                makePreviousParented = false;
                tokens->data[previousIndex].hasParent = true;
            }
        }
        else
        {
            SizeList* tokenIndices = &tokenIndicesByPrecedence[precedence];
            printf ("Evaluating precedence level %i, has %llu elements\n",
                    precedence, tokenIndices->count);
            error |= parseOperator (tokens, tokenIndices);
        }
    }

    for (OperatorPrecedence i = 0; i < OPERATOR_PRECEDENCE_COUNT; ++i)
    {
        deleteSizeList (&tokenIndicesByPrecedence[i]);
    }
    return error;
}

#endif

