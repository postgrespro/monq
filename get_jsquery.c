#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "monq_structures.h"
#include "get_jsquery.h"

//Function for concatination 1 string with form
char* 
sconcat1(char *form, char *s1, int plus) 
{
    size_t   len1 = strlen(s1);               
    char    *result = palloc(len1 + 1 + plus);

    sprintf(result,form,s1);  

    return result;
}

//Function for concatination 2 strings with form
static char* 
sconcat2(char *form, char *s1, char *s2, int plus) 
{
    size_t   len1 = strlen(s1);
    size_t   len2 = strlen(s2);                      
    char    *result = palloc(len1 + len2 + 1 + plus);
    
    sprintf(result,form,s1,s2);  

    return result;
}

//Function for concatination 3 strings with form
static char* 
sconcat3(char *form, char *s1, char *s2, char *s3, int plus) 
{
    size_t   len1 = strlen(s1);
    size_t   len2 = strlen(s2); 
    size_t   len3 = strlen(s3);                      
    char    *result = palloc(len1 + len2 + len3 + 1 + plus);

    sprintf(result, form, s1, s2, s3);  

    return result;
}

char *
getValueOperatorType(ValueOperatorType type)
{
    switch(type)
    {
        case _LESS :
            return "<";
        case _EQ :
        case _NOTEQ :
            return "=";
        case _LESSEQ :
            return "<=";
        case _GREAT :
            return ">";
        case _GREATEQ :
            return ">=";
        case _TYPE :
            return "";
        case _SIZE :
            return ".@# =";
        case _EXISTS :
            return "= *";
        default :
            elog(ERROR,"This value operator is not supported");
            break;
    }
}

// Return type in jsquery format 
char *
getValueType(char *type)
{
    if(strcmp(type,"\"string\"") == 0)              return " IS STRING";
    else if(
                strcmp(type, "\"double\"") == 0 || 
                strcmp(type, "\"int\"") == 0 || 
                strcmp(type, "\"long\"") == 0 || 
                strcmp(type, "\"decimal\"") == 0
            )                                             return " IS NUMERIC";
                             
    else if(strcmp(type, "\"array\"") == 0)         return " IS ARRAY";
    else if(strcmp(type, "\"object\"") == 0)        return " IS OBJECT";
    else if(strcmp(type, "\"bool\"") == 0)          return " IS BOOLEAN";
    else                                                  
        elog(ERROR, "Jsquery is not supported MongoDB %s value type", type);
}

char *
getValueOperator(char *key, ValueOperator *valOperator)
{
    char *opr = getValueOperatorType(valOperator->value_op);
    char *value = getLeafValue(valOperator->value);

    if(valOperator->value_op == _EXISTS)
        return valOperator->value->b ? sconcat2("%s %s", key, opr, 1) : sconcat2("NOT (%s %s)", key, opr, 7);
    else if(valOperator->value_op == _TYPE)
        return sconcat2("%s %s", key, getValueType(value), 1);
    else if(valOperator->value_op == _NOTEQ)
        return sconcat3("NOT (%s %s %s)", key, opr, value, 8);
    else
        return sconcat3("%s %s %s", key, opr, value, 2);
}

char *
getElemMatchOperator(char *key, ElemMatchOperator *elemMatchOperator)
{
    char *value = NULL;
    
    switch(elemMatchOperator->typeOfValue)
    {
        case E_EXPRESSION:
            value = getExpression(elemMatchOperator->expression);
            break;
        case E_OP_OBJECT:
            value = getOperatorObject("$",elemMatchOperator->operatorOpbject);
            break;
    }

    return sconcat2("%s.#:(%s)", key, value, 5);
}

char *
getOperator(char *key, Operator *operator)
{
    switch(operator->type)
    {
        case NOP :
            return getNotOperator(key, (NotOperator*) operator);
        case MOP :
            elog(ERROR, "MongoDB module operator is not supported by jsquery");
        case AOP :
            return getArrayOperator(key, (ArrayOperator*) operator);
        case VOP :
            return getValueOperator(key, (ValueOperator*) operator);
        case EOP :
            return getElemMatchOperator(key, (ElemMatchOperator*) operator);
        default  :
            elog(ERROR, "This mongoDB operator is not supported by jsquery");
    }
}

char * 
getNotOperator(char *key, NotOperator *notOperator)
{
    return sconcat1("NOT (%s)", getOperator(key,notOperator->op), 6);
}

char *
getOperatorObject(char *key, OperatorObject *opObject)
{
    char        *buf = "";
    ListCell    *cell;
    bool         first = true;

    foreach(cell, opObject->operatorList)
    {
        if(first)
        {
            buf = sconcat1("(%s)", getOperator(key, ((Operator *)lfirst(cell))), 2);
            first = false;
        }
        else           
            buf = sconcat2("%s AND (%s)", buf, getOperator(key, ((Operator *)lfirst(cell))), 7);
    }

    return buf;
}

char *
getLeafValueEq(char *key, LeafValue *leafValue)
{
    return sconcat2("%s = %s", key, getLeafValue(leafValue), 3);
}

char *
getLeafClauseValue(char *key, MValue *value)
{ 
    return (value->type ? getOperatorObject(key, value->oob) : getLeafValueEq(key, value->lv));
}

char *
getLeafClause(LeafClause *leafClause)
{
    return getLeafClauseValue(leafClause->key, leafClause->vl);
}

char *
getExpressionOperator(ExpressionOperatorType type)
{
    switch(type)
    {
        case _AND :
            return "AND";
        case _OR :
            return "OR";
        case _NOR :
            return "OR NOT";
        default :
            elog(ERROR,"This expression operator is not supported");
            break;
    }

    return NULL;
}

char *
getExpressionClause(ExpressionClause* expClause)
{
    char        *buf = "";
    char        *expOperator = getExpressionOperator(expClause->op);
    ListCell    *cell;
    bool         first = true;

    foreach(cell, expClause->expressionList)
    {
        if(first)
        {
            if(expClause->op == _NOR)
                buf = sconcat1("NOT (%s)", getExpression((Expression *)lfirst(cell)), 6);
            else
                buf = sconcat1("(%s)", getExpression((Expression *)lfirst(cell)), 2);

            first = false;
        }
        else
            buf = sconcat3("%s %s (%s)", buf, expOperator, getExpression((Expression *)lfirst(cell)), 4);
    }   

    return buf;
}

char *
getTextClause(TextClause *textClause)
{
    return sconcat1("* = %s", textClause->search_str, 4);
}

char *
getClause(Clause *clause)
{  
    switch(clause->type)
    {
        case LEAF :
            return getLeafClause((LeafClause*) clause);
        case COMMENT :
            elog(ERROR, "MongoDB comment clause is not supported by jsquery");
        case TEXT :
            return getTextClause((TextClause*) clause);
        case WHERE :
            elog(ERROR, "MongoDB where clause is not supported by jsquery");
        case EXPRESSION :
            return getExpressionClause((ExpressionClause*) clause);
        default:
            return NULL;
    }
}

char *
getExpression(Expression *expression)
{
    char        *buf = "";
    ListCell    *cell;
    bool         first = true;

    foreach(cell, expression->clauseList)
    {
        if (first)
        {
            buf = getClause((Clause *)lfirst(cell));
            first = false;
        }
        else
            buf = sconcat2("%s AND %s", buf, getClause((Clause *)lfirst(cell)), 5);
    }
    
    return buf;
}

char *
getLeafValue(LeafValue *value)
{
    switch(value->type)
    {
        case S :
            return value->str;
        case I :
            return value->i;
        case A :
            return sconcat1("[%s]", getArraySequence(value->ar), 2);
        case B :
            return (value->b ? "true" : "false");
        case D :
            return value->d;
        default :
            return NULL;
    }
}

char *
getArraySequence(MArray *marray)
{ 
    char        *buf = "";
    ListCell    *cell;
    bool         first = true;
    
    foreach(cell, marray->arrayList)
    {
        if(first)
        {
            buf = getLeafValue((LeafValue *)lfirst(cell));
            first = false;
        }
        else
            buf = sconcat2("%s, %s", getLeafValue((LeafValue *)lfirst(cell)), buf, 2);
    }

    return buf;
}

char *
getArrayOperator(char *key, ArrayOperator *arOperator)
{        
    char *ar = getArraySequence(arOperator->ar);

    switch(arOperator->array_op)
    {
        case _IN :
            return sconcat2("%s IN (%s)", key, ar, 6);
        case _NIN:
            return sconcat2("NOT (%s IN (%s))", key, ar, 12);
        case _ALL:
            return sconcat2("%s @> [%s]", key, ar, 6);
        default  :
            return NULL;
    } 
}

char *
getJsquery(MQuery *qu)
{
    return getExpression(qu->exp);
}