#include "parser.h"

LISTIFY_C(ParseItem);

enum
{
    PARSER_NONE,
    PARSER_REQUIRED_INTO_RHS,
    PARSER_REQUIRED_INTO_NEXT,
    PARSER_OPTIONAL_INTO_RHS,
    PARSER_OPTIONAL_INTO_NEXT,
};

enum
{
    PARSER_TOKEN_TYPE_VALUE,
    PARSER_TOKEN_TYPE_OPERATOR,
    PARSER_TOKEN_TYPE_KEYWORD,

    PARSER_TOKEN_PAIR_VALUE_VALUE
        = (PARSER_TOKEN_TYPE_VALUE << 4)
        |  PARSER_TOKEN_TYPE_VALUE,

    PARSER_TOKEN_PAIR_VALUE_OPERATOR
        = (PARSER_TOKEN_TYPE_VALUE << 4)
        |  PARSER_TOKEN_TYPE_OPERATOR,

    PARSER_TOKEN_PAIR_VALUE_KEYWORD
        = (PARSER_TOKEN_TYPE_VALUE << 4)
        |  PARSER_TOKEN_TYPE_KEYWORD,

    PARSER_TOKEN_PAIR_OPERATOR_VALUE
        = (PARSER_TOKEN_TYPE_OPERATOR << 4)
        |  PARSER_TOKEN_TYPE_VALUE,

    PARSER_TOKEN_PAIR_OPERATOR_OPERATOR
        = (PARSER_TOKEN_TYPE_OPERATOR << 4)
        |  PARSER_TOKEN_TYPE_OPERATOR,

    PARSER_TOKEN_PAIR_OPERATOR_KEYWORD
        = (PARSER_TOKEN_TYPE_OPERATOR << 4)
        |  PARSER_TOKEN_TYPE_KEYWORD,

    PARSER_TOKEN_PAIR_KEYWORD_VALUE
        = (PARSER_TOKEN_TYPE_KEYWORD << 4)
        |  PARSER_TOKEN_TYPE_VALUE,

    PARSER_TOKEN_PAIR_KEYWORD_OPERATOR
        = (PARSER_TOKEN_TYPE_KEYWORD << 4)
        |  PARSER_TOKEN_TYPE_OPERATOR,

    PARSER_TOKEN_PAIR_KEYWORD_KEYWORD
        = (PARSER_TOKEN_TYPE_KEYWORD << 4)
        |  PARSER_TOKEN_TYPE_KEYWORD,

};

void parsePrintToken (TokenList* tokens, Size i)
{
    Token* current = &tokens->data[i];

    bool isBracket = false;
    if (current->type == TOKEN_TYPE_OPERATOR)
    {
        OperatorFlags flags = operatorFlags[current->ident];
        isBracket = !!(flags & OPERAND_FLAG_OPEN_BRACKET);
    }

    if (isBracket)
    {
        Token* close = &tokens->data[current->rightIndex];
        printf ("%.*s", current->length, current->string);
        parsePrintToken (tokens, close->leftIndex);
        printf ("%.*s", close->length,   close->string);
        return;
    }

    if (current->leftIndex)
    {
        printf ("(");
        parsePrintToken (tokens, current->leftIndex);
        printf (")");
    }

    printf ("%.*s", current->length, current->string);

    if (current->rightIndex)
    {
        printf ("(");
        parsePrintToken (tokens, current->rightIndex);
        printf (")");
    }

    Size next = current->nextIndex;
    while (next)
    {
        printf (",");
        parsePrintToken (tokens, next);
        next = tokens->data[next].nextIndex;
    }
}

void parsePrint (TokenList* tokens, ParseItemList* list)
{
    if (list->count == 0)
    {   printf ("-empty-"); }
    for (Size i = 0; i < list->count; ++i)
    {
        if (i != 0)
        {   printf (" "); }
        printf ("%c", "VOK"[list->data[i].type]);
        printf ("`");
        parsePrintToken (tokens, list->data[i].index);
        printf ("`");
    }
    printf ("\n");
}

void parseReduce (TokenList* tokens, ParseItemList* list, OperatorPrecedence precedence)
{
    if (list->count < 2)
    {   return; }

    ParseItem*  leftItem = &list->data[list->count - 2];
    ParseItem* rightItem = &list->data[list->count - 1];
    Token* left  = &tokens->data[ leftItem->index];
    Token* right = &tokens->data[rightItem->index];
    U8 pairing = (leftItem->type << 4) | rightItem->type;

    printf ("Doing parseReduce: %s ", precedenceNames[precedence]);

    switch (pairing)
    {
        case PARSER_TOKEN_PAIR_VALUE_VALUE:
            printf ("V-V\n");
            if (precedence > OPERATOR_PRECEDENCE_ARGUMENT)
            {   return; }
            left->nextIndex    = rightItem->index;
            right->parentIndex =  leftItem->index;
            break;

        case PARSER_TOKEN_PAIR_OPERATOR_VALUE:
            printf ("V-O\n");
            if (precedence > leftItem->precedence)
            {   return; }
            left->rightIndex   = rightItem->index;
            right->parentIndex =  leftItem->index;
            leftItem->type = PARSER_TOKEN_TYPE_VALUE;
            break;

        case PARSER_TOKEN_PAIR_KEYWORD_KEYWORD:
            printf ("K-K\n");
            if (precedence > leftItem->precedence)
            {   return; }
            left->nextIndex    = rightItem->index;
            right->parentIndex =  leftItem->index;
            break;

        case PARSER_TOKEN_PAIR_KEYWORD_OPERATOR:
            printf ("K-O UNHANDLED\n");
            parsePrint (tokens, list);
            return;

        case PARSER_TOKEN_PAIR_VALUE_KEYWORD:
            printf ("V-K UNHANDLED\n");
            parsePrint (tokens, list);
            return;

        case PARSER_TOKEN_PAIR_VALUE_OPERATOR:
            printf ("V-O UNHANDLED\n");
            parsePrint (tokens, list);
            return;

        case PARSER_TOKEN_PAIR_KEYWORD_VALUE:
            printf ("K-V\n");
            if (precedence > OPERATOR_PRECEDENCE_SEMICOLON)
            {   return; }
            left->rightIndex   = rightItem->index;
            right->parentIndex =  leftItem->index;
            leftItem->type = PARSER_TOKEN_TYPE_VALUE;
            break;

        case PARSER_TOKEN_PAIR_OPERATOR_KEYWORD:
            printf ("O-K UNHANDLED\n");
            parsePrint (tokens, list);
            return;

        case PARSER_TOKEN_PAIR_OPERATOR_OPERATOR:
            printf ("O-O UNHANDLED\n");
            parsePrint (tokens, list);
            return;

    }

    --list->count;
    parsePrint (tokens, list);
    return parseReduce (tokens, list, precedence);
}

Size subParse (TokenList* tokens, Size start, Size end)
{
    printf ("subParse Called\n");
    ParseItemList list = createParseItemList (64);

    for (Size i = start; i < end; ++i)
    {
        Size currentIndex = i;
        Token* current = &tokens->data[currentIndex];
        printf ("\nsubParse interation begin\n");
        parsePrint (tokens, &list);

        ParseItem item = {.index = i};

        switch (current->type)
        {
            case TOKEN_TYPE_COMMENT:
                continue;

            default:
            {
                item.type = PARSER_TOKEN_TYPE_VALUE;
                item.precedence = OPERATOR_PRECEDENCE_ARGUMENT;
                appendParseItemList (&list, item);
            }
            break;

            case TOKEN_TYPE_OPERATOR:
            {
                Operator op = current->ident;
                OperatorFlags flags = operatorFlags[op];

                if (flags & OPERAND_LHS_BRACKET)
                {
                    Size closeIndex = current->rightIndex;
                    Size inner = subParse (tokens, currentIndex + 1, closeIndex);
                    tokens->data[closeIndex].leftIndex = inner;
                    tokens->data[inner].parentIndex = closeIndex;
                    item.type = PARSER_TOKEN_TYPE_VALUE;
                    item.precedence = OPERATOR_PRECEDENCE_ARGUMENT;
                    appendParseItemList (&list, item);
                    i = closeIndex;
                    break;
                }

                OperatorPrecedence precedence = operatorPrecedences[op];
                parseReduce (tokens, &list, precedence);
                if (flags & OPERAND_MASK_LHS_SEARCH)
                {
                    ParseItem* top = &list.data[list.count - 1];
                    if (top->type == PARSER_TOKEN_TYPE_VALUE)
                    {
                        --list.count;
                        current->leftIndex = top->index;
                        tokens->data[top->index].parentIndex = currentIndex;
                    }
                }
                if (flags & OPERAND_MASK_RHS_SEARCH)
                {
                    item.type = PARSER_TOKEN_TYPE_OPERATOR;
                    item.precedence = precedence;
                }
                else
                {
                    item.type = PARSER_TOKEN_TYPE_VALUE;
                    item.precedence = OPERATOR_PRECEDENCE_ARGUMENT;
                }
                appendParseItemList (&list, item);
            }
            break;

            case TOKEN_TYPE_KEYWORD:
            {
                printf ("Keyword encountered\n");
                OperatorPrecedence precedence = OPERATOR_PRECEDENCE_SEMICOLON;
                parseReduce (tokens, &list, precedence);
                if (keywordHasBlock[current->ident])
                {
                    Size nextIndex = i;
                    while (true)
                    {
                        if (tokens->data[nextIndex].type == TOKEN_TYPE_OPERATOR
                        &&  tokens->data[nextIndex].ident == OPERATOR_BLOCK_OPEN)
                        {
                            break;
                        }
                        ++nextIndex;
                    }
                    printf ("Block found at %llu\n", nextIndex);
                    Size inner = subParse (tokens, currentIndex + 1, nextIndex);
                    current->leftIndex = inner;
                    tokens->data[inner].parentIndex = currentIndex;
                    i = nextIndex - 1;
                }

                item.type = PARSER_TOKEN_TYPE_KEYWORD;
                item.precedence = precedence;
                appendParseItemList (&list, item);
            }
            break;

        }
    }

    printf ("Exit reduce\n");
    parseReduce (tokens, &list, 0);
    printf ("Leaving subParse\n");
    if (list.count == 0)
    {   return 0; }
    return list.data[0].index;
}

RynError parse (TokenList* tokens)
{
    subParse (tokens, 1, tokens->count - 1);
    return SUCCESS;
}

