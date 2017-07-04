#ifndef _JSQUERY_CALLS_H_
#define _JSQUERY_CALLS_H_

	extern Datum callJsquery_in(char *query);

	extern Datum callJsquery_out(Datum jsquery_query);

	extern Datum callJsquery_jsonb_exec(Datum jsonb_data, Datum jsquery_query);

#endif