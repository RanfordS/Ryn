#include "parser.h"

// At any point, if the left hand side doesn't take a right,
// and current doesn't take a left, then we are chain's next.

// return is the next token index

LISTIFY_C(Size);

enum
{
    PARSER_TOKEN_CLASS_NONE,
    PARSER_TOKEN_CLASS_VALUE,
    PARSER_TOKEN_CLASS_OPERATOR,
    PARSER_TOKEN_CLASS_KEYWORD,
};
typedef U8 ParserTokenClass;

enum
{
    PARSER_TOKEN_POS_LHS,
    PARSER_TOKEN_POS_RHS,
    PARSER_TOKEN_POS_NEXT,
};
typedef U8 ParserTokenPosition;

typedef struct s_ParserStackItem
{
    Size index;
    ParserTokenClass class;
    ParserTokenPosition position;
}
ParserStackItem;

static void tokenApply (TokenList* tokens, Size parent, Size child, ParserTokenPosition pos)
{
    if (!child)
    {   return; }

    tokens->data[child].parentIndex = parent;
    switch (pos)
    {
        case PARSER_TOKEN_POS_LHS:
            tokens->data[parent].leftIndex = child;
            break;

        case PARSER_TOKEN_POS_RHS:
            tokens->data[parent].rightIndex = child;
            break;

        case PARSER_TOKEN_POS_NEXT:
            tokens->data[parent].nextIndex = child;
            break;
    }
}

static ParserTokenClass tokenClassify (const Token* token, bool isHead)
{
    printf ("  > tokenClassify: `%.*s` isHead=%i\n", token->length, token->string, isHead);

    if (token->type == TOKEN_TYPE_KEYWORD
    &&  keywordHasBlock[token->ident])
    {
        printf ("    Is keyword\n");
        return PARSER_TOKEN_CLASS_KEYWORD;
    }
    
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
        if (betaClass == PARSER_TOKEN_CLASS_KEYWORD
        &&  precedence > OPERATOR_PRECEDENCE_SEMICOLON)
        {
            printf ("  Keyword Rule\n");
            alpha->nextIndex = betaIndex;
            beta->parentIndex = alphaIndex;
            --stack->count;
        }
        else
        {   return; }
    }
}

Size fetchBlock (TokenList* tokens, Size start)
{
    printf ("> Fetching block\n");
    for (Size i = start; i < tokens->count; ++i)
    {
        Token* token = &tokens->data[i];
        if (token->type != TOKEN_TYPE_OPERATOR)
        {   continue; }

        Operator op = token->ident;
        if (op == OPERATOR_BLOCK_OPEN
        ||  op == OPERATOR_END_STATEMENT)
        {
            return i;
        }

        OperatorFlags flags = operatorFlags[op];

        // if we open brackets, skip over them
        if (flags & OPERAND_LHS_BRACKET)
        {
            i = token->rightIndex;
        }
        // if we have a close bracket, we should stop
        else if (flags & OPERAND_RHS_BRACKET)
        {
            return 0;
        }

    }
    return 0;
}

static Size subParse (TokenList* tokens, Size start, Size end)
{
    printf ("subParse called between %llu and %llu\n", start, end);
    if (end <= start)
    {   return 0; }
    SizeList stack = createSizeList (64);

    for (Size i = start; i < end; ++i)
    {
        Size currentIndex = i;
        Token* current = &tokens->data[currentIndex];
        printf ("Token %llu: `%.*s`\n", currentIndex, current->length, current->string);

        if (current->type == TOKEN_TYPE_COMMENT)
        {   continue; }
        printf ("Not a comment\n");

        // is this a token that opens a block
        if (current->type == TOKEN_TYPE_KEYWORD
        &&  keywordHasBlock[current->ident])
        {
            printf ("Token is a keyword that takes a block\n");
            Size blockIndex = fetchBlock (tokens, currentIndex);
            Size argumentIndex = subParse (tokens, currentIndex + 1, blockIndex);
            tokenApply (tokens, currentIndex, argumentIndex, PARSER_TOKEN_POS_LHS);
            tokenApply (tokens, currentIndex, blockIndex, PARSER_TOKEN_POS_RHS);

            i = blockIndex - 1;
            appendSizeList (&stack, currentIndex);
            printf ("Block index = %llu, Argument index = %llu\n", blockIndex, argumentIndex);
            continue;
        }

        // handle brackets
        if (current->type == TOKEN_TYPE_OPERATOR
        &&  operatorFlags[current->ident] & OPERAND_LHS_BRACKET)
        {
            printf ("Token is a bracket, process accordingly\n");
            Size closeIndex = current->rightIndex;
            Size innerIndex = subParse (tokens, currentIndex + 1, closeIndex);
            tokens->data[innerIndex].parentIndex = closeIndex;
            tokens->data[closeIndex].leftIndex = innerIndex;
            i = closeIndex;
            continue;
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
                    top->parentIndex = currentIndex;
                    --stack.count;
                }
            }
        }
        else
        {
            printf ("Token is considered a value\n");
        }

        printf ("Add to stack\n");
        appendSizeList (&stack, currentIndex);
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
    printf ("End of subparse, collapsing stack\n");

    // collapse all remaining
    parseReduce (tokens, &stack, OPERATOR_PRECEDENCE_COUNT);

    printf ("Final count = %llu\n\n", stack.count);
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

