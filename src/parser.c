#include "parser.h"

#if 1

// At any point, if the left hand side doesn't take a right,
// and current doesn't take a left, then we are chain's next.

// return is the next token index

LISTIFY_C(Size);

enum
{
    PARSER_TOKEN_CLASS_VALUE,
    PARSER_TOKEN_CLASS_OPERATOR,
};
typedef U8 ParserTokenClass;

static ParserTokenClass tokenClassify (const Token* token, bool isHead)
{
    printf ("  > tokenClassify: `%.*s` isHead=%i\n", token->length, token->string, isHead);
    if (token->type != TOKEN_TYPE_OPERATOR)
    {
        printf ("    Not operator, thus value\n");
        return PARSER_TOKEN_CLASS_VALUE;
    }

    printf ("    Is operator, thus get flags\n");
    Operator operator = token->ident;
    OperatorFlags flags = operatorFlags[operator];

    if (isHead)
    {
        printf ("    Token is head, check left\n");
        printf ("    Flags=%i, Masked=%i\n",
            flags, flags & OPERAND_FLAG_LHS_POSSIBLE);
        printf ("    leftIndex=%llu\n", token->leftIndex);
        if (flags & OPERAND_FLAG_LHS_POSSIBLE
        &&  token->leftIndex == 0)
        {
            printf ("    LHS possible and not yet set\n");
            return PARSER_TOKEN_CLASS_OPERATOR;
        }
    }
    else
    {
        printf ("    Token is not head, check right\n");
        printf ("    Flags=%i, Masked=%i\n",
            flags, flags & OPERAND_FLAG_RHS_POSSIBLE);
        printf ("    rightIndex=%llu\n", token->rightIndex);
        if (flags & OPERAND_FLAG_RHS_POSSIBLE
        &&  token->rightIndex == 0)
        {
            printf ("    LHS possible and not yet set\n");
            return PARSER_TOKEN_CLASS_OPERATOR;
        }
    }

    return PARSER_TOKEN_CLASS_VALUE;
}

void parseReduce (TokenList* tokens, SizeList* stack, OperatorPrecedence precedence)
{
    printf ("> parseReduce called\n");
    // possible stacks (top only):
    //
    //    op     v   op       op
    // 0 [`(a)+`,`b`]<`+` -> [`((a)+(b))+`]  alpha 7 == gamma 7
    // 1 [`(a)+`,`b`]<`*` -> [`(a)+`,`(b)*`] alpha 7 >= gamma 6
    // 2 [`(a)*`,`b`]<`+` -> [`((a)*(b))+`]  alpha 6 <= gamma 7 same as 0
    // 4 [`(a)*`,`b`]<`*` -> [`((a)*(b))*`]  alpha 6 == gamma 6 identical to 0
    //
    // [`((a).(b))*`,`c`]<`+` -> [`(((a).(b))*(c))+`]
    // [`((a).(b))+`,`c`]<`*` -> [`((a).(b))+`,`(c)*`]
    // [`(a)*`,`(b).`,`c`]<`+` -> [`(a)*`,`(b).(c)`]<`+`
    // -> [`(a)*((b).(c))`]<`+` -> [`((a)*((b).(c)))+`]
    // [`((a)*(b))+`,`c`]<`.` -> [`((a)*(b))+`,`(c).`]
    // [`(a)+`,`(b).`,`c`]<`*` -> [`(a)+`,`((b).(c))*`]
    // [`(a)+`,`(b)*`,`c`]<`.` -> [`(a)+`,`(b)*`,`(c).`]
    //
    // [`(a)*`,`-`]<`b` -> [`(a)*`,`-`,`b`]
    // [`(a)*`,`-`,`b`]<`*` -> [`(a)*`,`-`,`(b)*`]
    // [`(a)*`,`-`,`(b)*`]<`c` -> [`(a)*`,`-`,`(b)*`,`c`]
    // -> [`(a)*`,`-`,`(b)*`,`c`]<`+` -> [`(a)*`,`-`,`(b)*(c)`]<`+`
    // -> [`(a)*`,`-((b)*(c))`]<`+` -> [`(a)*(-((b)*(c)))`]<`+`
    // -> [`((a)*(-((b)*(c))))+`]
    //
    // [`(a)+`,`(b)*`,`(c).`,`d`]-end ->
    //
    // `.` precedence = 1
    // `*` precedence = 6
    // `+` precedence = 7

    // as long as we have two tokens to work with, we can try to reduce the stack
    while (2 <= stack->count)
    {
        printf ("  Iteration\n");
        Size alphaIndex = stack->data[stack->count - 2];
        Size betaIndex  = stack->data[stack->count - 1];
        Token* alpha = &tokens->data[alphaIndex];
        Token* beta  = &tokens->data[betaIndex];
        ParserTokenClass alphaClass = tokenClassify (alpha, false);
        ParserTokenClass betaClass  = tokenClassify (beta,  false);

        printf ("  alphaClass = %s\n",
                alphaClass == PARSER_TOKEN_CLASS_OPERATOR
                ? "Operator"
                : "Value");

        printf ("  betaClass = %s\n",
                alphaClass == PARSER_TOKEN_CLASS_OPERATOR
                ? "Operator"
                : "Value");

        if (alphaClass == PARSER_TOKEN_CLASS_OPERATOR
        &&  betaClass  == PARSER_TOKEN_CLASS_VALUE)
        {
            printf ("  Operator Rule\n");
            OperatorPrecedence alphaPrecedence
                = operatorPrecedences[alpha->ident];
            if (alphaPrecedence <= precedence)
            {
                alpha->rightIndex = betaIndex;
                beta->parentIndex = alphaIndex;
                --stack->count;
            }
            else
            {   return; }
        }
        else
        if (alphaClass == PARSER_TOKEN_CLASS_VALUE
        &&  betaClass  == PARSER_TOKEN_CLASS_VALUE
        &&  precedence >= OPERATOR_PRECEDENCE_ARGUMENT)
        {
            printf ("  Argument Rule\n");
            alpha->nextIndex  = betaIndex;
            beta->parentIndex = alphaIndex;
            --stack->count;
        }
        else
        {   return; }
    }
}

static Size subParse (TokenList* tokens, Size start, Size end)
{
    printf ("subParse called between %llu and %llu\n", start, end);
    SizeList stack = createSizeList (64);

    for (Size i = start; i < end; ++i)
    {
        Token* current = &tokens->data[i];
        printf ("Token %llu: `%.*s`\n", i, current->length, current->string);

        if (current->type == TOKEN_TYPE_COMMENT)
        {   continue; }
        printf ("Not a comment\n");

        // handle brackets
        if (current->type == TOKEN_TYPE_OPERATOR
        &&  operatorFlags[current->ident] & OPERAND_LHS_BRACKET)
        {
            printf ("Token is a bracket, process accordingly\n");
            Size closeIndex = current->rightIndex;
            Size inner = subParse (tokens, i + 1, closeIndex);
            tokens->data[closeIndex].leftIndex = inner;
        }

        ParserTokenClass class = tokenClassify (current, true);
        if (class == PARSER_TOKEN_CLASS_OPERATOR)
        {
            printf ("Token is considered an operator\n");
            OperatorPrecedence precedence = operatorPrecedences[current->ident];
            parseReduce (tokens, &stack, precedence);
            if (stack.count > 0)
            {
                Size topIndex = stack.data[stack.count - 1];
                Token* top = &tokens->data[topIndex];
                ParserTokenClass leftClass = tokenClassify (top, false);

                if (leftClass == PARSER_TOKEN_CLASS_VALUE)
                {
                    current->leftIndex = topIndex;
                    top->parentIndex = i;
                    --stack.count;
                }
            }
        }
        else
        {
            printf ("Token is considered a value\n");
        }

        printf ("Add to stack\n");
        appendSizeList (&stack, i);
        printf ("Current stack:\n[");
        for (Size j = 0; j < stack.count; ++j)
        {
            if (j > 0)
            {
                printf (",");
            }
            printf ("%llu", stack.data[j]);
        }
        printf ("]\n\n");
    }

    // collapse all remaining
    parseReduce (tokens, &stack, OPERATOR_PRECEDENCE_COUNT);

    Size root = stack.data[0];
    deleteSizeList (&stack);
    return root;
}

RynError parse (TokenList* tokens)
{
    printf ("Outer parse function called\n\n");
    subParse (tokens, 1, tokens->count - 1);
    return SUCCESS;
}

#elif 0

#include "listify.h"

LISTIFY_H(Size);
LISTIFY_C(Size);

//TODO: Add provisions around brackets
//eg `a[b]` on `b` where `[` is marked as used and thus grabs something it shouldn't
//same issue with `]`
Size getLeftTokenIndex (const TokenList* tokens, Size i, OperatorPrecedence precedence)
{
    printf ("Getting token left of %llu: ", i);
    while (0 < --i)
    {
        Token* token = &tokens->data[i];
        if (token->type == TOKEN_TYPE_OPERATOR
        &&  operatorPrecedences[token->ident] > precedence
        &&  token->rightIndex != 0)
        {
            printf ("discarded `%.*s` by handedness rules\n", token->length, token->string);
            return 0;
        }
        if (token->type != TOKEN_TYPE_COMMENT
        &&  token->parentIndex == 0)
        {
            printf ("found at %llu\n", i);
            return i;
        }
    }
    printf ("none found\n");
    return 0;
}

Size getRightTokenIndex (const TokenList* tokens, Size i, OperatorPrecedence precedence)
{
    printf ("Getting token right of %llu: ", i);
    while (++i < tokens->count - 1)
    {
        Token* token = &tokens->data[i];
        if (token->type == TOKEN_TYPE_OPERATOR
        &&  operatorPrecedences[token->ident] > precedence
        &&  token->leftIndex != 0)
        {
            printf ("discarded `%.*s` by handedness rules\n", token->length, token->string);
            return 0;
        }
        if (token->type != TOKEN_TYPE_COMMENT
        &&  token->parentIndex == 0)
        {
            printf ("found at %llu\n", i);
            return i;
        }
    }
    printf ("none found\n");
    return 0;
}

static Size seekSide (const TokenList* tokens, OperatorFlags maskedFlags, Size thisIndex, int dir, bool* applyOperator, OperatorPrecedence precedence)
{
    printf ("seekSide for %llu, dir=%i\n", thisIndex, dir);
    if (!maskedFlags)
    {   return 0; }

    Size otherIndex = 0;
    if (dir < 0)
    {
        otherIndex = getLeftTokenIndex (tokens, thisIndex, precedence);
    }
    else
    {
        otherIndex = getRightTokenIndex (tokens, thisIndex, precedence);
    }

    bool isRequired = !!(maskedFlags & OPERAND_MASK_REQUIRED);
    bool isPossible = !!(maskedFlags & OPERAND_MASK_POSSIBLE);
    bool wasFound = !!otherIndex;
    printf ("isRequired=%i, isPossible=%i, wasFound=%i\n", isRequired, isPossible, wasFound);
    if (isRequired
    &&  isPossible != wasFound)
    {
        printf ("Discounted as required was different from found\n");
        *applyOperator = false;
    }
    if (wasFound
    &&  tokens->data[otherIndex].type == TOKEN_TYPE_OPERATOR)
    {
        printf ("Considering exeption\n");
        OperatorFlags otherFlags = operatorFlags[tokens->data[otherIndex].ident];
        OperatorFlags thisFlags  = operatorFlags[tokens->data[thisIndex ].ident];
        if (dir < 0)
        {
            printf ("Other is to the left\n");
            if ((otherFlags & OPERAND_MASK_RHS_SEARCH) == OPERAND_RHS_REQUIRED
            ||  (thisFlags  & OPERAND_MASK_LHS_SEARCH) == OPERAND_LHS_OPTIONAL)
            {
                printf ("Exception applies\n");
                return 0;
            }
            printf ("Keep\n");
        }
        else
        {
            printf ("Other is to the right\n");
            if ((otherFlags & OPERAND_MASK_LHS_SEARCH) == OPERAND_LHS_REQUIRED
            ||  (thisFlags  & OPERAND_MASK_RHS_SEARCH) == OPERAND_RHS_OPTIONAL)
            {
                printf ("Exception applies\n");
                return 0;
            }
            printf ("Keep\n");
        }
        //return 0;
    }
    return otherIndex;
}

// note for all parser functions contained within, return errors can be safely
// propagated with |= as there is only two codes, all other types of errors are
// applied to tokens

//TODO: handle the case of `;` where the right operand is optional
static RynError parserHandleSide (TokenList* tokens, Size parentIndex, Size childIndex, RynError missingOperandError)
{
    Token* parent = &tokens->data[parentIndex];
    if (!childIndex)
    {
        parent->error = missingOperandError;
        return ERROR_PARSER_HAD_ERROR;
    }

    Token* childToken = &tokens->data[childIndex];
    childToken->parentIndex = parentIndex;
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
        OperatorFlags flags = operatorFlags[token->ident];
        OperatorPrecedence precedence = operatorPrecedences[token->ident];
        printf ("Parsing operator 0x%04llx `%.*s`\n", tokenIndex, token->length, token->string);

        bool applyOperator = true;
        Size leftIndex  = seekSide (tokens, flags & OPERAND_MASK_LHS_SEARCH,
            tokenIndex, -1, &applyOperator, precedence);
        Size rightIndex = seekSide (tokens, flags & OPERAND_MASK_RHS_SEARCH,
            tokenIndex, +1, &applyOperator, precedence);
        printf ("\n");

        token->leftIndex = leftIndex;
        error |= parserHandleSide
            (tokens, tokenIndex, leftIndex, ERROR_PARSER_NO_LHS);
        token->rightIndex = rightIndex;
        error |= parserHandleSide
            (tokens, tokenIndex, rightIndex, ERROR_PARSER_NO_RHS);
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
            // we getRight until we hit an operator of higher precedence
            // also, we loop backwards to avoid parent-marking issues
            for (Size i = tokens->count - 2; i >= 1; --i)
            {
                printf ("\nToken %llu\n", i);
                Token* token = &tokens->data[i];
                if (token->parentIndex != 0
                ||  token->type == TOKEN_TYPE_COMMENT)
                {   continue; }
                printf ("  had no parent\n");

                if (token->type == TOKEN_TYPE_OPERATOR)
                {
                    printf ("  is an operator\n");
                    OperatorPrecedence p = operatorPrecedences[token->ident];
                    if (p > OPERATOR_PRECEDENCE_ARGUMENT) { continue; };
                    printf ("  but of lower precedence\n");
                }

                Size rightIndex = getRightTokenIndex (tokens, i, OPERATOR_PRECEDENCE_ARGUMENT);
                if (!rightIndex) { continue; }
                printf ("  right index found\n");

                Token* rightToken = &tokens->data[rightIndex];
                /*
                if (rightToken->type == TOKEN_TYPE_OPERATOR)
                {
                    printf ("  right index is an operator\n");
                    OperatorPrecedence precedence =
                        operatorPrecedences[rightToken->ident];
                    if (precedence > OPERATOR_PRECEDENCE_ARGUMENT) { continue; }
                    printf ("  but it is of lower precedence\n");
                }
                */
                printf ("Thus it is our next token\n");

                rightToken->parentIndex = i;
                token->nextIndex = rightIndex;
            }
        }
        else
        {
            SizeList* tokenIndices = &tokenIndicesByPrecedence[precedence];
            printf ("\nEvaluating precedence level %i, has %llu elements\n",
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

