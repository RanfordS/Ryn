#pragma once
#include "core.h"
#include "operator.h"
#include "token.h"
#include "keyword.h"

typedef struct s_ParseItem
{
    Size index;
    U8 type;
    OperatorPrecedence precedence;
}
ParseItem;

LISTIFY_H(ParseItem);

extern RynError parse (TokenList* tokens);

