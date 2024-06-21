#pragma once
#include "core.h"
#include "operator.h"
#include "token.h"
#include "keyword.h"

LISTIFY_H(Size);

extern RynError parse (TokenList* tokens);

