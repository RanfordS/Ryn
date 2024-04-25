#include "operator.h"

const char* const operators[OPERATOR_COUNT] =
{
#define X(str,name,pos) [OPERATOR_##name] = str,
    OPERATOR_X_LIST
#undef X
};

const uint8_t operatorFlags[OPERATOR_COUNT] =
{
#define X(str,name,pos) [OPERATOR_##name] = pos,
    OPERATOR_X_LIST
#undef X
};
