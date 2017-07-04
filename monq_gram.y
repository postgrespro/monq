%{
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>

    #include "monq_structures.h"

    #include "get_jsquery.h"
    #include "create_mquery.h"
    #include "jsquery_calls.h"
    #include "delete_mquery.h"

    #define DatumGetMQueryP(d) ((MQuery*)DatumGetPointer(PG_DETOAST_DATUM(d)))
    #define PG_RETURN_MQUERY(p) PG_RETURN_POINTER(p)
    #define PG_GETARG_MQUERY(x) DatumGetMQueryP(PG_GETARG_DATUM(x))

    MQuery *RET;

    typedef struct yy_buffer_state *YY_BUFFER_STATE;
    extern int yylex();
    extern void yyerror(char *s);
    extern int yyparse();
    extern YY_BUFFER_STATE yy_scan_string(char * str);
    extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

    void 
    yyerror(char *s) 
    { 
        elog(ERROR,"%s",s);
        exit(0);
    }
%}

/* Types of query tree nodes and leafs */
%union 
{
    MQuery                    *mquery;
    Expression                *exp;
    Clause                    *cl;
    MValue                    *vl;
    LeafValue                 *lv;
    List                      *list;
    WhereClauseValue          *wcv;
    char                      *strval;
    int                        intval;
    double                     dubval;
    MArray                    *arrval;
    bool                       boolval;
    ArrayOperatorType         aop_type;
    ExpressionOperatorType     exop_type;
    ValueOperatorType        valop_type;
    OperatorObject           *oob;
    Operator                  *op;
}

%type<mquery>     QUERY
%type<exp>        EXPRESSION
%type<cl>         CLAUSE TEXT_CLAUSE EXPRESSION_TREE_CLAUSE LEAF_CLAUSE COMMENT_CLAUSE WHERE_CLAUSE
%type<vl>         VALUE 

%type<strval>     KEY KEY_STRING
%type<op>         OPERATOR
%type<oob>        OPEARATOR_OBJECT

%type<strval>     LSCOPE RSCOPE COMMA
    
%type<list>       OPERATOR_LIST LEAF_VALUE_LIST EXPRESSION_LIST CLAUSE_LIST

%type<wcv>        WHERE_CLAUSE_VALUE
%type<strval>     WHERE_OPERATOR
%token            WHERE_OPERATOR

/* OPERATORS */

/* Tree operator */
%type<exop_type>    TREE_OPERATOR OR NOR AND
%token              OR NOR AND

/* Leaf value operator */
 %type<valop_type>    EQ LESS GREAT LESSEQ GREATEQ NOTEQ TYPE SIZE EXISTS NOT VALUE_OPERATOR
 %token               EQ NOTEQ LESS LESSEQ GREAT GREATEQ TYPE SIZE EXISTS NOT

/* Array operator */
%type<aop_type>   IN NIN ALL ARRAY_OPERATOR
%token            IN NIN ALL

/* Mod operator */
%type<strval>     MOD_OPERATOR 
%type<lv>         DIVISOR REMAINDER
%token            MOD_OPERATOR

/* Comment clause operator */
%type<strval>     COMMENT_OPERATOR
%token            COMMENT_OPERATOR

/* Text clause operator */
%type<strval>     DIACRITIC_SENSITIVE_OPERATOR CASE_SENSITIVE_OPERATOR LANGUAGE_OPERATOR SEARCH_OPERATOR TEXT_OPERATOR
%token            DIACRITIC_SENSITIVE_OPERATOR CASE_SENSITIVE_OPERATOR LANGUAGE_OPERATOR SEARCH_OPERATOR TEXT_OPERATOR

/* Type of values */
%type<lv> LEAF_VALUE
%type<strval> INT
%type<strval> STRING
%type<strval> DOUBLE
%type<arrval> ARRAY
%type<boolval> BOOLEAN    
%token INT STRING DOUBLE BOOLEAN KEY_STRING

/* Scope types  */
%token LSCOPE RSCOPE COMMA LSQBRACKET RSQBRACKET LRBRACKET RRBRACKET

%start QUERY

%%
QUERY                   : EXPRESSION {$$ = createQuery($1); RET=$$; }
                        ;

EXPRESSION              : LSCOPE CLAUSE_LIST RSCOPE { $$ = createExpression($2); }
                        ;

CLAUSE_LIST             : CLAUSE COMMA CLAUSE_LIST  { $$ = addClause($1, $3); }
                        | CLAUSE                    { $$ = lappend(NULL, $1); }
                        ;

CLAUSE                  : LEAF_CLAUSE             
                        | COMMENT_CLAUSE         
                        | WHERE_CLAUSE            
                        | EXPRESSION_TREE_CLAUSE  
                        | TEXT_CLAUSE             
                        ;


/* TEXT CLAUSE SECTION */

TEXT_CLAUSE             : LSCOPE TEXT_OPERATOR EQ LSCOPE SEARCH_OPERATOR EQ KEY 
                            RSCOPE RSCOPE { $$ = createTextClause($7, false, "", false, false); }
                        | LSCOPE TEXT_OPERATOR EQ LSCOPE SEARCH_OPERATOR EQ KEY COMMA
                            LANGUAGE_OPERATOR EQ STRING COMMA
                            CASE_SENSITIVE_OPERATOR EQ BOOLEAN COMMA
                            DIACRITIC_SENSITIVE_OPERATOR EQ BOOLEAN RSCOPE RSCOPE { $$ = createTextClause($7, false, $11, $15, $19); }
                        ;

/* END OF SECTION */

/*WHERE CLAUSE SECTION*/
                
WHERE_CLAUSE            : LSCOPE WHERE_OPERATOR EQ WHERE_CLAUSE_VALUE RSCOPE { $$ = createWhereClause($4); }
                        ;

WHERE_CLAUSE_VALUE      : KEY { $$ = stringToWhereClauseValue($1); }
                        ;
/* END OF SECTION */   

/*COMMENT CLAUSE SECTION*/
COMMENT_CLAUSE          : LSCOPE COMMENT_OPERATOR EQ STRING RSCOPE { $$ = createCommentClause($2, $4); }
                        ;
/* END OF SECTION */

/*TREE CLAUSE SECTION*/

EXPRESSION_TREE_CLAUSE  : TREE_OPERATOR EQ LSQBRACKET EXPRESSION_LIST RSQBRACKET { $$ = createExpressionTreeClause($1, $4); }
                        | LSCOPE EXPRESSION_TREE_CLAUSE RSCOPE                   { $$ = $2; }
                        ;

EXPRESSION_LIST         : EXPRESSION                       { $$ = lcons($1, NULL); }
                        | EXPRESSION COMMA EXPRESSION_LIST { $$ = addExpression($1, $3); }
                        ;

TREE_OPERATOR           : OR | AND | NOR ;

/* END OF SECTION */

/* LEAF CLAUSE SECTION */
LEAF_CLAUSE             : KEY EQ VALUE { $$ = createLeafClause($1, $3); }
                        ;

KEY                     : STRING
                        | KEY_STRING
                        ;

VALUE                   : LEAF_VALUE       { $$ = createLeafValueValue($1); }
                        | OPEARATOR_OBJECT { $$ = createOperatorObjectValue($1); }
                        ;

OPEARATOR_OBJECT        : LSCOPE OPERATOR_LIST RSCOPE { $$ = createOperatorObject($2); }
                        ;

OPERATOR_LIST           : OPERATOR                     { $$ = lappend(NULL, $1); }
                        | OPERATOR COMMA OPERATOR_LIST { $$ = addOperator($1, $3); }
                        ;

OPERATOR                : VALUE_OPERATOR EQ LEAF_VALUE                                  { $$ = createValueOperator($1, $3); }
                        | ARRAY_OPERATOR EQ ARRAY                                       { $$ = createArrayOperator($1, $3); }
                        | MOD_OPERATOR EQ LSQBRACKET DIVISOR COMMA REMAINDER RSQBRACKET { $$ = createModOperator($4, $6); }
                        | NOT EQ LSCOPE OPERATOR RSCOPE                                 { $$ = createNotOperator($4); }
                        ;

VALUE_OPERATOR          : EQ | NOTEQ | LESS | LESSEQ | GREAT | GREATEQ | TYPE | SIZE | EXISTS 
                        ;

DIVISOR                 : LEAF_VALUE
                        ;

REMAINDER               : LEAF_VALUE
                        ;

ARRAY                   : LSQBRACKET LEAF_VALUE_LIST RSQBRACKET {$$ = createNewArray($2); }
                        ;

ARRAY_OPERATOR          : IN | NIN | ALL
                        ;

LEAF_VALUE_LIST         : LEAF_VALUE                         { $$ = lcons($1, NULL); }
                        | LEAF_VALUE COMMA LEAF_VALUE_LIST   { $$ = addArrayElement($1, $3); }
                        ;

LEAF_VALUE              : INT         { $$ = createIntegerValue($1); }
                        | STRING      { $$ = createStringValue($1); }
                        | KEY_STRING  { $$ = createStringValue(sconcat1("\"%s\"",$1,2)); }
                        | DOUBLE      { $$ = createDoubleValue($1); }
                        | ARRAY       { $$ = createArrayValue($1); }
                        | BOOLEAN     { $$ = createBooleanValue($1); }
                        ;

/* END OF SECTION */
%%

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(mquery_in);
Datum
mquery_in(PG_FUNCTION_ARGS)
{
    char            *input=PG_GETARG_CSTRING(0);
    YY_BUFFER_STATE  buffer = yy_scan_string(input);
    
    yyparse();
    yy_delete_buffer(buffer);
       
    PG_RETURN_MQUERY(RET);
}

/*  
    Transform mongoDB query tree to jsquery query
    and call jsquery_out function of jsquery extension.
*/
PG_FUNCTION_INFO_V1(mquery_out);
Datum
mquery_out(PG_FUNCTION_ARGS)
{
    MQuery      *input=PG_GETARG_MQUERY(0);
    char        *JSQUERY_QUERY = getJsquery(input);
    Datum        jsquery_object = callJsquery_in(JSQUERY_QUERY);

    deleteMquery(input);

    PG_RETURN_CSTRING(callJsquery_out(jsquery_object));
}

/*  
    Function for mongoDB exec. 
    On input of function pass jsonb and mquery.
    Mdbquery transform to jsquery and pass to 
    son_jsquery_exec function of JSquery extension.
*/
PG_FUNCTION_INFO_V1(json_mquery_exec);
Datum
json_mquery_exec(PG_FUNCTION_ARGS)
{
    Jsonb       *jb = PG_GETARG_JSONB(0);
    MQuery      *mq = PG_GETARG_MQUERY(1);
    char        *JSQUERY_QUERY = getJsquery(mq);
    Datum        js_query = callJsquery_in(JSQUERY_QUERY);

    deleteMquery(mq);

    PG_FREE_IF_COPY(jb, 0);
    PG_FREE_IF_COPY(mq, 1);

    return callJsquery_jsonb_exec(PointerGetDatum(jb), js_query);
}

/*  
    Function for mongoDB exec. 
    On input of function pass jsonb and mquery.
    Mdbquery transform to jsquery and pass to 
    son_jsquery_exec function of JSquery extension.
*/
PG_FUNCTION_INFO_V1(mquery_json_exec);
Datum
mquery_json_exec(PG_FUNCTION_ARGS)
{
    MQuery      *mq = PG_GETARG_MQUERY(0);
    Jsonb       *jb = PG_GETARG_JSONB(1);
    char        *JSQUERY_QUERY = getJsquery(mq);
    Datum        js_query = callJsquery_in(JSQUERY_QUERY);

    deleteMquery(mq);

    PG_FREE_IF_COPY(mq, 0);
    PG_FREE_IF_COPY(jb, 1);

    return callJsquery_jsonb_exec(PointerGetDatum(jb), js_query);
} 