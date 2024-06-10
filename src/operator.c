#include "operator.h"
#include "stringlib.h"

const char* const operators[OPERATOR_COUNT] =
{
#define X(str,name,pos,context,prec) [OPERATOR_##name] = str,
    OPERATOR_X_LIST
#undef X
};

const OperatorPosition operatorPositions[OPERATOR_COUNT] =
{
#define X(str,name,pos,context,prec) [OPERATOR_##name] = OPERATOR_POSITION_##pos,
    OPERATOR_X_LIST
#undef X
};

const OperatorContext operatorContexts[OPERATOR_COUNT] =
{
#define X(str,name,pos,context,prec) [OPERATOR_##name] = OPERATOR_CONTEXT_##context,
    OPERATOR_X_LIST
#undef X
};

const OperatorPrecedence operatorPrecedences[OPERATOR_COUNT] =
{
#define X(str,name,pos,context,prec) [OPERATOR_##name] = OPERATOR_PRECEDENCE_##prec,
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

bool operatorMatchContext (Operator searchStart, OperatorContext context, Operator* result)
{
    for (Operator i = searchStart; i < OPERATOR_COUNT; ++i)
    {
        if ((strcmp (operators[searchStart], operators[i]) == 0)
        &&  (operatorContexts[i] == context))
        {
            *result = i;
            return true;
        }
    }
    return false;
}
