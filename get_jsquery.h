#ifndef _GET_QUERY_H_
#define _GET_QUERY_H_

#include "monq_structures.h"

	extern char* sconcat1(char *form, char *s1, int plus);

	extern char *getJsquery(MQuery * query);

	extern char *getExpression(Expression * expression);

	extern char *getClause(Clause *clause);

	// Expression tree clause functions
	extern char *getExpressionClause(ExpressionClause* expClause);

	extern char *getExpressionOperator(ExpressionOperatorType type);

	// Text clause functions
	extern char *getTextClause(TextClause* textClause);

	// Leaf clause functions
	extern char *getLeafClause(LeafClause *leafClause);

	extern char *getLeafClauseValue(char *key, MValue *value);

	// Leaf value eq
	extern char *getLeafValueEq(char *key, LeafValue *leafValue);

	// Leaf value operators
	extern char *getOperatorObject(char *key, OperatorObject *opObject);

	// Operators
	extern char *getOperator(char *key, Operator *operator);

	//Not operator
	extern char *getNotOperator(char *key, NotOperator *notOperator);

	extern char *getElemMatchOperator(char *key, ElemMatchOperator *elemMatchOperator);

	//Arrray operator
	extern char *getArrayOperator(char *key, ArrayOperator *arOperator);

	extern char *getArraySequence(MArray *marray);

	extern char *getLeafValue(LeafValue *value);

	// Value operator
	extern char *getValueOperator(char *key, ValueOperator *valOperator);

	extern char *getValueOperatorType(ValueOperatorType type);

	extern char *getValueType(char *type);

#endif