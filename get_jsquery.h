#ifndef _GET_QUERY_H_
#define _GET_QUERY_H_

#include "monq_structures.h"

	extern char* sconcat1(char *form, char *s1, int plus);

	extern char *getJsquery(MQuery * query);

	extern char *getExpression(Expression * expression);

	extern char *getClause(Clause *clause);

	extern char *getExpressionClause(ExpressionClause* expClause);

	extern char *getExpressionOperator(ExpressionOperatorType type);

	extern char *getTextClause(TextClause* textClause);

	extern char *getLeafClause(LeafClause *leafClause);

	extern char *getLeafClauseValue(char *key, MValue *value);

	extern char *getLeafValueEq(char *key, LeafValue *leafValue);

	extern char *getOperatorObject(char *key, OperatorObject *opObject);

	extern char *getOperator(char *key, Operator *operator);

	extern char *getNotOperator(char *key, NotOperator *notOperator);

	extern char *getElemMatchOperator(char *key, ElemMatchOperator *elemMatchOperator);

	extern char *getArrayOperator(char *key, ArrayOperator *arOperator);

	extern char *getArraySequence(MArray *marray);

	extern char *getLeafValue(LeafValue *value);

	extern char *getValueOperator(char *key, ValueOperator *valOperator);

	extern char *getValueOperatorType(ValueOperatorType type);

	extern char *getValueType(char *type);

#endif