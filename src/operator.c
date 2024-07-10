#include "operator.h"
#include "stringlib.h"

const char* const operators[OPERATOR_COUNT] =
{
#define X(str,name,lhsType,rhsType,context,prec) \
    [OPERATOR_##name] = str,
    OPERATOR_X_LIST
#undef X
};

const OperatorFlags operatorFlags[OPERATOR_COUNT] =
{
#define X(str,name,lhsType,rhsType,context,prec) \
    [OPERATOR_##name] = (OPERAND_LHS_##lhsType | OPERAND_RHS_##rhsType),
    OPERATOR_X_LIST
#undef X
};

const OperatorContext operatorContexts[OPERATOR_COUNT] =
{
#define X(str,name,lhsType,rhsType,context,prec) \
    [OPERATOR_##name] = OPERATOR_CONTEXT_##context,
    OPERATOR_X_LIST
#undef X
};

const OperatorPrecedence operatorPrecedences[OPERATOR_COUNT] =
{
#define X(str,name,lhsType,rhsType,context,prec) \
    [OPERATOR_##name] = OPERATOR_PRECEDENCE_##prec,
    OPERATOR_X_LIST
#undef X
};

const char* precedenceNames[OPERATOR_PRECEDENCE_COUNT] =
{
    [OPERATOR_PRECEDENCE_NA] = "NA",
    [OPERATOR_PRECEDENCE_SEMICOLON] = "SEMICOLON",
    [OPERATOR_PRECEDENCE_ASSIGN] = "ASSIGN",
    [OPERATOR_PRECEDENCE_COMMA] = "COMMA",
    [OPERATOR_PRECEDENCE_SLICE] = "SLICE",
    [OPERATOR_PRECEDENCE_ARRAY] = "ARRAY",
    [OPERATOR_PRECEDENCE_LOGICAL_OR] = "LOGICAL_OR",
    [OPERATOR_PRECEDENCE_LOGICAL_XOR] = "LOGICAL_XOR",
    [OPERATOR_PRECEDENCE_LOGICAL_AND] = "LOGICAL_AND",
    [OPERATOR_PRECEDENCE_COMPARE] = "COMPARE",
    [OPERATOR_PRECEDENCE_RANGE] = "RANGE",
    [OPERATOR_PRECEDENCE_BIT_OR] = "BIT_OR",
    [OPERATOR_PRECEDENCE_BIT_XOR] = "BIT_XOR",
    [OPERATOR_PRECEDENCE_BIT_AND] = "BIT_AND",
    [OPERATOR_PRECEDENCE_SHIFT] = "SHIFT",
    [OPERATOR_PRECEDENCE_MATH_AS] = "MATH_AS",
    [OPERATOR_PRECEDENCE_MATH_DM] = "MATH_DM",
    [OPERATOR_PRECEDENCE_ARGFEED] = "ARGFEED",
    [OPERATOR_PRECEDENCE_ARGUMENT] = "ARGUMENT",
    [OPERATOR_PRECEDENCE_ARGSPEC] = "ARGSPEC",
    [OPERATOR_PRECEDENCE_PREFIX] = "PREFIX",
    [OPERATOR_PRECEDENCE_SUFFIX] = "SUFFIX",
};

bool operatorFind (Operator searchStart, U8 opLen, const char* opStr, Operator* result)
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
        if (strcmp (operators[searchStart], operators[i]) == 0
        &&  (operatorContexts[i] == context
            || operatorContexts[i] == OPERATOR_CONTEXT_ALWAYS))
        {
            *result = i;
            return true;
        }
    }
    return false;
}

bool operatorMatchFlagsAndContext (Operator searchStart, OperatorFlags flags, OperatorContext context, Operator* result)
{
    for (Operator i = searchStart; i < OPERATOR_COUNT; ++i)
    {
        if (strcmp (operators[searchStart], operators[i]) == 0
        &&  (operatorContexts[i] == context
            || operatorContexts[i] == OPERATOR_CONTEXT_ALWAYS)
        &&  operatorFlags[i] == flags)
        {
            *result = i;
            return true;
        }
    }
    return false;
}

