#ifndef _CREATE_MQUERY_H_
#define _CREATE_MQUERY_H_

extern MArray *createNewArray(List *arrayList);

extern List *addArrayElement(LeafValue * value, List *arrayList);

extern Operator *createNotOperator(Operator *op);

extern Operator *createModOperator(LeafValue *divisor, LeafValue *remainder);

extern Operator *createArrayOperator(ArrayOperatorType op, MArray *ar);

extern Operator *createValueOperator(ValueOperatorType op, LeafValue *value);

extern Operator *createElemMatchOperatorExpression(Expression *expression);

extern Operator *createElemMatchOperatorOpObject(OperatorObject *oob);

extern LeafValue *createStringValue(char *str);

extern LeafValue *createDoubleValue(char* d);

extern LeafValue *createIntegerValue(char* i);

extern LeafValue *createArrayValue(MArray *ar);

extern LeafValue *createBooleanValue(bool b);

extern List *addOperator(Operator *op, List *operatorList);

extern OperatorObject *createOperatorObject(List *operatorList);

extern MValue *createOperatorObjectValue(OperatorObject *oob);

extern MValue *createLeafValueValue(LeafValue *lv);

extern Clause *createLeafClause(char* key, MValue *vl);

extern Clause *createCommentClause(char *op, char *str);

extern WhereClauseValue *stringToWhereClauseValue(char *str);

extern Clause *createWhereClause(WhereClauseValue *wcv);

extern List *addClause(Clause *clause, List *clauseList);

extern Expression *createExpression(List *clauseList);

extern List *addExpression(Expression *exp, List *expressionList);

extern Clause *createExpressionTreeClause(ExpressionOperatorType op, List *expressionList);

extern Clause *createTextClause(char* search_str, bool lang_op, char* lang_str, bool case_sense, bool diacr_sense);

extern MQuery *createQuery(Expression *exp);

#endif