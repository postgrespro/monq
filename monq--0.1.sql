\echo Use "CREATE EXTENSION MDBQS" to load this file. \quit

CREATE TYPE mquery;

CREATE FUNCTION mquery_in(cstring) RETURNS mquery
AS '$libdir/monq'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mquery_out(mquery) RETURNS cstring
AS '$libdir/monq'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION json_mquery_exec(jsonb, mquery) RETURNS bool
AS '$libdir/monq'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION mquery_json_exec(mquery, jsonb) RETURNS bool
AS '$libdir/monq'
LANGUAGE C STRICT IMMUTABLE;

CREATE TYPE mquery (
	INTERNALLENGTH = -1,
	INPUT = mquery_in,
	OUTPUT = mquery_out,
	STORAGE = extended
);

CREATE OPERATOR <=> (
	LEFTARG = jsonb,
	RIGHTARG = mquery,
	PROCEDURE = json_mquery_exec,
	COMMUTATOR = '<=>',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR <=> (
	LEFTARG = mquery,
	RIGHTARG = jsonb,
	PROCEDURE = mquery_json_exec,
	COMMUTATOR = '<=>',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

