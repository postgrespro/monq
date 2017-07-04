#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "monq_structures.h"

#include "parser/parse_func.h"

#include "access/htup_details.h"
#include "access/sysattr.h"
#include "access/xact.h"
#include "catalog/dependency.h"
#include "catalog/indexing.h"
#include "catalog/namespace.h"
#include "catalog/objectaccess.h"
#include "catalog/pg_collation.h"
#include "catalog/pg_depend.h"
#include "catalog/pg_extension.h"
#include "catalog/pg_namespace.h"
#include "catalog/pg_type.h"
#include "commands/alter.h"
#include "commands/comment.h"
#include "commands/defrem.h"
#include "commands/extension.h"
#include "commands/schemacmds.h"
#include "funcapi.h"
#include "mb/pg_wchar.h"
#include "miscadmin.h"
#include "nodes/makefuncs.h"
#include "storage/fd.h"
#include "tcop/utility.h"
#include "utils/acl.h"
#include "utils/builtins.h"
#include "utils/fmgroids.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/rel.h"
#include "utils/snapmgr.h"
#include "utils/tqual.h"

#include "jsquery_calls.h"

#define EXTENSION_NAME   "jsquery"

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

    extensionOid = get_extension_oid(EXTENSION_NAME, false);
    shemaOid = get_extension_schema(extensionOid);
    return list_make2(makeString(get_namespace_name(shemaOid)), makeString(functionName));
}

Datum
callJsquery_in(char *query)
{
    Oid         functionOid;
    FmgrInfo   *procedure = palloc0(sizeof(FmgrInfo));
    Oid         funcargtypes[1];

    funcargtypes[0] = CSTRINGOID;
    functionOid = LookupFuncName(get_function_name("jsquery_in"), 1, funcargtypes, false);

    if (OidIsValid(functionOid))
    {
        fmgr_info(functionOid, procedure); 
        return FunctionCall1(procedure, PointerGetDatum(query));
    }
    else
        elog(ERROR,"Oid of %s is not valid", "jsquery_in");
    
}

Datum
callJsquery_out(Datum jsquery_query)
{
    Oid         functionOid;
    FmgrInfo   *procedure = palloc0(sizeof(FmgrInfo));
    Oid         funcargtypes[1];

    funcargtypes[0] = CSTRINGOID;
    functionOid = LookupFuncName(get_function_name("jsquery_out"), 1, funcargtypes, false);

    if (OidIsValid(functionOid))
    {
        fmgr_info(functionOid, procedure); 
        return FunctionCall1(procedure, jsquery_query);
    }
    else
        elog(ERROR,"Oid of %s is not valid", "jsquery_out");
}

Datum
callJsquery_jsonb_exec(Datum jsonb_data, Datum jsquery_query)
{
    Oid         functionOid;
    FmgrInfo   *procedure = palloc0(sizeof(FmgrInfo));
    Oid         funcargtypes[2];

    funcargtypes[0] = JSONBOID;
    funcargtypes[1] = TypenameGetTypid("jsquery");
    functionOid = LookupFuncName( get_function_name("json_jsquery_exec"), 2, funcargtypes, false);

    if (OidIsValid(functionOid))
    {
        fmgr_info(functionOid, procedure); 
        return FunctionCall2(procedure, jsonb_data, jsquery_query);
    }
    else
        elog(ERROR,"Oid of %s is not valid", "json_jsquery_exec");
}