#include "operator.h"
#include "stringlib.h"

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



bool operatorFind (Operator searchStart, uint8_t opLen, const char* opStr, Operator* result)
{
    for (Operator i = searchStart; i < OPERATOR_COUNT; ++i)
    {
        if (nstringMatchCstring (opLen, opStr, operators[i]))
        {
            *result = i;
            return true;
        }
    }
    return false;
}
