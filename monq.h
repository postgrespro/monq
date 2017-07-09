#ifndef _MONQ_H_
#define _MONQ_H_

#include "monq_structures.h"


#define DatumGetMQueryP(d) ((MQuery*)DatumGetPointer(PG_DETOAST_DATUM(d)))
#define PG_RETURN_MQUERY(p) PG_RETURN_POINTER(p)
#define PG_GETARG_MQUERY(x) DatumGetMQueryP(PG_GETARG_DATUM(x))


char *getJsquery(MQuery * query);
MQuery* parsemquery(char *str);
char *sconcat1(char *form, char *s1, int plus);

#endif