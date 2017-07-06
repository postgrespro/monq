#ifndef _DELETE_MQUERY_H_
#define _DELETE_MQUERY_H_

	extern void deleteValueType(char *value_type);

	extern void deleteValueOperator(ValueOperator *vop);

	extern void deleteOperator(Operator *op);

	extern void deleteNotOperator(NotOperator *op);

	extern void deleteElemMatchOperator(ElemMatchOperator *elemMatchOperator);

	extern void deleteOperatorObject(OperatorObject *op_object);

	extern void deleteLeafValueEq(LeafValue *lv);

	extern void deleteLeafClauseValue(MValue *val);

	extern void deleteLeafClause(LeafClause *lc);

	extern void deleteExpressionClause(ExpressionClause* expClause);

	extern void deleteTextClause(TextClause *tClause);

	extern void deleteClause(Clause *cl);

	extern void deleteExpression(Expression * ex);

	extern void deleteLeafValue(LeafValue *value);

	extern void deleteArraySequence(MArray *ar);

	extern void deleteArrayOperator(ArrayOperator *aop);

	extern void deleteMquery(MQuery *qu);
	
#endif