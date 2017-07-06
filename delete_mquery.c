#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "monq_structures.h"

#include "access/gin.h"
#include "utils/numeric.h"

#include "delete_mquery.h"


void
deleteValueType(char *value_type)
{
   free(value_type);
}

void
deleteValueOperator(ValueOperator *vop)
{   
    deleteLeafValue(vop->value);
    pfree(vop);
}

void
deleteOperator(Operator *operator)
{
    switch(operator->type)
    {
        case NOP :
            deleteNotOperator((NotOperator*) operator );
            break;
        case AOP :
            deleteArrayOperator((ArrayOperator*) operator );
            break;
        case VOP :
            deleteValueOperator((ValueOperator*) operator );
            break;
        case EOP :
            deleteElemMatchOperator((ElemMatchOperator*) operator);
            break;
        case MOP :
            break;
    }
}

void
deleteElemMatchOperator(ElemMatchOperator *elemMatchOperator)
{
    switch(elemMatchOperator->typeOfValue)
    {
        case E_EXPRESSION:
            deleteExpression(elemMatchOperator->expression);
            break;
        case E_OP_OBJECT:
            deleteOperatorObject(elemMatchOperator->operatorOpbject);
            break;
    }
}

void 
deleteNotOperator(NotOperator *op)
{
    deleteOperator(op->op);
    pfree(op);
}

void
deleteOperatorObject(OperatorObject *op_object)
{
    List        *operatorList;
    ListCell    *cell;
    
    operatorList = op_object->operatorList;

    foreach(cell, operatorList)
        deleteOperator((Operator *)lfirst(cell));
        
    pfree(op_object->operatorList);
    pfree(op_object);
}

void
deleteLeafClauseValue(MValue *value)
{ 
    value->type ? deleteOperatorObject(value->oob) : deleteLeafValue(value->lv);
}

void
deleteLeafClause(LeafClause *lc)
{
    deleteLeafClauseValue(lc->vl);
    pfree(lc);
}

void
deleteExpressionClause(ExpressionClause* expClause)
{
    List        *expressionList;
    ListCell    *cell;

    expressionList = expClause->expressionList;

    foreach(cell, expressionList)
        deleteExpression((Expression *)lfirst(cell));
    
    pfree(expressionList);
    pfree(expClause);
}

void
deleteTextClause(TextClause *textClause)
{
    pfree(textClause);
}

void
deleteClause(Clause *clause)
{  
    switch(clause->type)
    {
        case LEAF :
            deleteLeafClause((LeafClause*) clause);
            break;
        case TEXT :
            deleteTextClause((TextClause*) clause);
            break;
        case EXPRESSION :
            deleteExpressionClause((ExpressionClause*) clause);
            break;
        default :
            break;
    }
}

void
deleteExpression(Expression * expression)
{
    List        *clauseList = expression->clauseList;
    ListCell    *cell;
   
    foreach(cell, clauseList)
        deleteClause((Clause *)lfirst(cell));
    
    pfree(expression);
}

void
deleteLeafValue(LeafValue *value)
{
    if(value->type == A)
        deleteArraySequence(value->ar);
    pfree(value);
}

void
deleteArraySequence(MArray *marray)
{ 
    List        *arrayList = marray->arrayList;
    ListCell    *cell;
    
    foreach(cell, arrayList)
        deleteLeafValue((LeafValue *)lfirst(cell));

    pfree(arrayList);
    pfree(marray);
}

void
deleteArrayOperator(ArrayOperator *arrayOperator)
{        
    deleteArraySequence(arrayOperator->ar);
    pfree(arrayOperator);  
}

void
deleteMquery(MQuery *query)
{
    deleteExpression(query->exp);
    pfree(query);
}