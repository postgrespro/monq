MonQ - postgreSQL extension for MongoDB query support
=====================================================

Introduction
------------

MonQ – is a postgreSQL extension which allow use mongoDB query language
to query jsonb data type, introduced in PostgreSQL release 9.4.

The main task of this extension is to provide apportunity to make 
queries on mongodb query language, translate their in to jsquery query
language and return result of them. 

MonQ is released as mquery data type and <=> match operator for jsonb.

Availability
------------

MonQ is realized as an extension and not available in default PostgreSQL
installation. It is available from
[github](https://github.com/NikitOS94/MonQ)
and supports PostgreSQL 9.4+.

Installation
------------

MonQ is PostgreSQL extension which requires JsQuery extension 
[github](https://github.com/NikitOS94/MonQ) and PostgreSQL 9.4 or higher.

Before build and install you should ensure following:
    
 * PostgreSQL version is 9.4 or higher.
 * Installed JsQuery extension.
 * You have development package of PostgreSQL installed or you built
   PostgreSQL from source.
 * You have flex and bison installed on your system. 
 * Your PATH variable is configured so that pg\_config command available, or set PG_CONFIG variable.
    
Typical installation procedure may look like this:
    
    $ git clone https://github.com/nikitos94/monq.git
    $ cd monq
    $ make USE_PGXS=1
    $ sudo make USE_PGXS=1 install
    $ make USE_PGXS=1 installcheck
    $ psql DB -c "CREATE EXTENSION monq;"

MonQ opportunities
------------------

Functionality of monq Is limited by opportunities JsQuery but support main part of MongoDB query operators.

### Comparison operators:
* All operators is supported.
### Logical operators:
* All operators is supported.
### Element operators:
* All operators is supported.
### Evaluation operators:
* `$mod` - not supported;
* `$regex` - not supported;
* `$text` - supported;
* `$where` - not supported;
### Bitwise operators:
* All operators is not supported.
### Array operators:
* All operators is supported.
### Comment operators:
* All operators is not supported.
### Geospatial operators:
* All operators is not supported.
### Projextion operators:
* All operators is not supported.

* `$eq` - supported;
* `$ne` - supported;
* `$lt` - supported;
* `$lte` - supported;
* `$eq` - supported;
* `$eq` - supported;
Monq extension contains `mquery` datatype which represents MongoDB query  whole JSON query
as a single value. The query is an expression on JSON-document values.
MonQ   

 * `x = "abc"` – value of key "x" is equal to "abc";
 * `$ @> [4, 5, "zzz"]` – the JSON document is an array containing values
    4, 5 and "zzz";
 * `"abc xyz" >= 10` – value of key "abc xyz" is greater than or equal to 10;
 * `volume IS NUMERIC` – type of key "volume" is numeric.
 * `$ = true` – the whole JSON document is just a true.
 * `similar_ids.@# > 5` – similar\_ids is an array or object of length greater
   than 5;
 * `similar_product_ids.# = "0684824396"` – array "similar\_product\_ids"
   contains string "0684824396".
 * `*.color = "red"` – there is object somewhere which key "color" has value
   "red".
 * `foo = *` – key "foo" exists in object.

Path selects set of JSON values to be checked using given operators. In
the simplest case path is just an key name. In general path is key names and
placeholders combined by dot signs. Path can use following placeholders:

 * `#` – any index of array;
 * `#N` – N-th index of array;
 * `%` – any key of object;
 * `*` – any sequence of array indexes and object keys;
 * `@#` – length of array or object, could be only used as last component of
    path;
 * `$` – the whole JSON document as single value, could be only the whole path.

Expression is true when operator is true against at least one value selected
by path.

Key names could be given either with or without double quotes. Key names
without double quotes shouldn't contain spaces, start with number or concur
with jsquery keyword.

The supported binary operators are:

 * Equality operator: `=`;
 * Numeric comparison operators: `>`, `>=`, `<`, `<=`;
 * Search in the list of scalar values using `IN` operator;
 * Array comparison operators: `&&` (overlap), `@>` (contains),
   `<@` (contained in).

The supported unary operators are:

 * Check for existence operator: `= *`;
 * Check for type operators: `IS ARRAY`, `IS NUMERIC`, `IS OBJECT`, `IS STRING`
   and `IS BOOLEAN`.

Expressions could be complex. Complex expression is a set of expressions
combined by logical operators (`AND`, `OR`, `NOT`) and grouped using braces.

Examples of complex expressions are given below.

 * `a = 1 AND (b = 2 OR c = 3) AND NOT d = 1`
 * `x.% = true OR x.# = true`

Prefix expressions are expressions given in the form path (subexpression).
In this case path selects JSON values to be checked using given subexpression.
Check results are aggregated in the same way as in simple expressions.

 * `#(a = 1 AND b = 2)` – exists element of array which a key is 1 and b key is 2
 * `%($ >= 10 AND $ <= 20)` – exists object key which values is between 10 and 20

Path also could contain following special placeholders with "every" semantics:

 * `#:` – every indexes of array;
 * `%:` – every key of object;
 * `*:` – every sequence of array indexes and object keys.

Consider following example.

    %.#:($ >= 0 AND $ <= 1)

This example could be read as following: there is at least one key which value
is array of numerics between 0 and 1.

We can rewrite this example in the following form with extra braces.

    %(#:($ >= 0 AND $ <= 1))

The first placeholder `%` checks that expression in braces is true for at least
one value in object. The second placeholder `#:` checks value to be array and
all its elements satisfy expressions in braces.

We can rewrite this example without `#:` placeholder as follows.

    %(NOT #(NOT ($ >= 0 AND $ <= 1)) AND $ IS ARRAY)

In this example we transform assertion that every element of array satisfy some
condition to assertion that there is no one element which doesn't satisfy the
same condition.

Some examples of using paths are given below.

 * `numbers.#: IS NUMERIC` – every element of "numbers" array is numeric.
 * `*:($ IS OBJECT OR $ IS BOOLEAN)` – JSON is a structure of nested objects
   with booleans as leaf values.
 * `#:.%:($ >= 0 AND $ <= 1)` – each element of array is object containing
   only numeric values between 0 and 1.
 * `documents.#:.% = *` – "documents" is array of objects containing at least
   one key.
 * `%.#: ($ IS STRING)` – JSON object contains at least one array of strings.
 * `#.% = true` – at least one array element is objects which contains at least
   one "true" value.

Usage of path operators and braces need some explanation. When same path
operators are used multiple times they may refer different values while you can
refer same value multiple time by using braces and `$` operator. See following
examples.

 * `# < 10 AND # > 20` – exists element less than 10 and exists another element
   greater than 20.
 * `#($ < 10 AND $ > 20)` – exists element which both less than 10 and greater
   than 20 (impossible).
 * `#($ >= 10 AND $ <= 20)` – exists element between 10 and 20.
 * `# >= 10 AND # <= 20` – exists element great or equal to 10 and exists
   another element less or equal to 20. Query can be satisfied by array with
   no elements between 10 and 20, for instance [0,30].

Same rules apply when you search inside objects and branchy structures.

Type checking operators and "every" placeholders are useful for document
schema validation. JsQuery matchig operator `@@` is immutable and can be used
in CHECK constraint. See following example.

```sql
CREATE TABLE js (
    id serial,
    data jsonb,
    CHECK (data @@ '
        name IS STRING AND
        similar_ids.#: IS NUMERIC AND
        points.#:(x IS NUMERIC AND y IS NUMERIC)'::jsquery));
```

In this example check constraint validates that in "data" jsonb column:
value of "name" key is string, value of "similar_ids" key is array of numerics,
value of "points" key is array of objects which contain numeric values in
"x" and "y" keys.

See our
[pgconf.eu presentation](http://www.sai.msu.su/~megera/postgres/talks/pgconfeu-2014-jsquery.pdf)
for more examples.

Contribution
------------
Please, notice, that MonQ is still under development and while it's stable
and tested, it may contains some bugs. Don't hesitate to raise issues at 
github with your bug reports.

If you're lacking of some functionality in MonQ and feeling power to 
implement it then you're welcome to make pull requests.
