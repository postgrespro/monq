#include <stdio.h>

#include "postgres.h"
#include "fmgr.h"

#include "access/genam.h"
#include "access/heapam.h"
#include "access/htup_details.h"
#include "access/sysattr.h"
#include "catalog/indexing.h"
#include "catalog/namespace.h"
#include "catalog/pg_extension.h"
#include "catalog/pg_type.h"
#include "commands/extension.h"
#include "parser/parse_func.h"
#include "utils/fmgroids.h"
#include "utils/jsonb.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/rel.h"

#include "monq.h"


#define JSQUERY_EXTENSION_NAME   "jsquery"

FmgrInfo   *funcJJE = NULL;
FmgrInfo   *funcJI = NULL;
FmgrInfo   *funcJO = NULL;

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(mquery_in);
PG_FUNCTION_INFO_V1(mquery_out);
PG_FUNCTION_INFO_V1(json_mquery_exec);
PG_FUNCTION_INFO_V1(mquery_json_exec);

static Datum callJsquery_in(char *query);
static Datum callJsquery_out(Datum jsquery_query);
static Datum callJsquery_jsonb_exec(Datum jsonb_data, Datum jsquery_query);


Datum
mquery_in(PG_FUNCTION_ARGS)
{
    char    *input=PG_GETARG_CSTRING(0);
    MQuery  *res;

    res = parsemquery(input);
    res->jsQuery = callJsquery_in(getJsquery(res));
    deleteMquery(res);

    PG_RETURN_MQUERY(res);
}

/*  
 * Transform mongoDB query tree to jsquery query
 * and call jsquery_out function of jsquery extension.
 */
Datum
mquery_out(PG_FUNCTION_ARGS)
{
    MQuery      *input = PG_GETARG_MQUERY(0);

    PG_RETURN_CSTRING(callJsquery_out(input->jsQuery));
}

/*  
 * jsonb <=> mquery operator func
 *   This function accepts jsonb and mquery. After mquery object is parsed we
 *   transform it to jsquery and then pass it to json_jsquery_exec() function
 *   of the JSquery extension.
 */
Datum
json_mquery_exec(PG_FUNCTION_ARGS)
{
    Jsonb       *jb = PG_GETARG_JSONB(0);
    MQuery      *mq = PG_GETARG_MQUERY(1);

    return callJsquery_jsonb_exec(PointerGetDatum(jb), mq->jsQuery);
}

/*  
 * mquery <=> jsonb operator func
 *   This function accepts mquery and jsonb. After mquery object is parsed we
 *   transform it to jsquery and then pass it to json_jsquery_exec() function
 *   of the JSquery extension.
 */
Datum
mquery_json_exec(PG_FUNCTION_ARGS)
{
    MQuery      *mq = PG_GETARG_MQUERY(0);
    Jsonb       *jb = PG_GETARG_JSONB(1);

    return callJsquery_jsonb_exec(PointerGetDatum(jb), mq->jsQuery);
}

static Oid
get_extension_schema(Oid ext_oid)
{
    Oid         result;
    Relation    rel;
    SysScanDesc scandesc;
    HeapTuple   tuple;
    ScanKeyData entry[1];

    rel = heap_open(ExtensionRelationId, AccessShareLock);

    ScanKeyInit(&entry[0],
                ObjectIdAttributeNumber,
                BTEqualStrategyNumber, F_OIDEQ,
                ObjectIdGetDatum(ext_oid));

    scandesc = systable_beginscan(rel, ExtensionOidIndexId, true,
                                  NULL, 1, entry);

    tuple = systable_getnext(scandesc);

    if (HeapTupleIsValid(tuple))
        result = ((Form_pg_extension) GETSTRUCT(tuple))->extnamespace;
    else
        result = InvalidOid;

    systable_endscan(scandesc);

    heap_close(rel, AccessShareLock);

    return result;
}

static List *
get_function_name(char *functionName)
{
    Oid         extensionOid;
    Oid         shemaOid;

    extensionOid = get_extension_oid(JSQUERY_EXTENSION_NAME, false);
    shemaOid = get_extension_schema(extensionOid);
    return list_make2(makeString(get_namespace_name(shemaOid)), makeString(functionName));
}

static Datum
callJsquery_in(char *query)
{
    if(funcJI == NULL)
    {
        MemoryContext   oldcxt;
        FmgrInfo       *finfo;
        Oid             functionOid;
        Oid             funcargtypes[1];

        oldcxt = MemoryContextSwitchTo(TopMemoryContext);

        finfo = palloc0(sizeof(FmgrInfo));
        funcargtypes[0] = CSTRINGOID;
        functionOid = LookupFuncName(get_function_name("jsquery_in"), 1, funcargtypes, false);
        fmgr_info(functionOid, finfo);

        MemoryContextSwitchTo(oldcxt);
        funcJI = finfo;
    }
    return FunctionCall1(funcJI, PointerGetDatum(query));    
}

static Datum
callJsquery_out(Datum jsquery_query)
{
    if(funcJO == NULL)
    {
        MemoryContext   oldcxt;
        FmgrInfo       *finfo;
        Oid             functionOid;
        Oid             funcargtypes[1];

        oldcxt = MemoryContextSwitchTo(TopMemoryContext);

        finfo = palloc0(sizeof(FmgrInfo));
        funcargtypes[0] = CSTRINGOID;
        functionOid = LookupFuncName(get_function_name("jsquery_out"), 1, funcargtypes, false);
        fmgr_info(functionOid, finfo);

        MemoryContextSwitchTo(oldcxt);
        funcJO = finfo;
    }

    return FunctionCall1(funcJO, jsquery_query);
}

static Datum
callJsquery_jsonb_exec(Datum jsonb_data, Datum jsquery_query)
{
    if(funcJJE == NULL)
    {
        MemoryContext   oldcxt;
        FmgrInfo       *finfo;
        Oid             functionOid;
        Oid             funcargtypes[2];

        oldcxt = MemoryContextSwitchTo(TopMemoryContext);

        finfo = palloc0(sizeof(FmgrInfo));
        funcargtypes[0] = JSONBOID;
        funcargtypes[1] = TypenameGetTypid("jsquery");
        functionOid = LookupFuncName( get_function_name("json_jsquery_exec"), 2, funcargtypes, false);
        fmgr_info(functionOid, finfo);

        MemoryContextSwitchTo(oldcxt);
        funcJJE = finfo;
    }
    return FunctionCall2(funcJJE, jsonb_data, jsquery_query);
}