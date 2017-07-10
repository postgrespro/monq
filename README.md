## MonQ 
postgreSQL extension for MongoDB query support

### Introduction

MonQ – is a postgreSQL extension which allow use mongoDB query language
to query jsonb data type, introduced in PostgreSQL release 9.4.

The main task of this extension is to provide apportunity to make 
queries on mongodb query language, translate their in to jsquery query
language and return result of them. 

MonQ is released as mquery data type and <=> match operator for jsonb.

### Availability

MonQ is realized as an extension and not available in default PostgreSQL
installation. It is available from
[github](https://github.com/NikitOS94/MonQ)
and supports PostgreSQL 9.4+.

### Installation

MonQ is PostgreSQL extension which requires JsQuery extension 
[github](https://github.com/NikitOS94/MonQ) and PostgreSQL 9.4 or higher.

Before build and install you should ensure following:
    
 * PostgreSQL version is 9.4 or higher.
 * Installed JsQuery extension.
 * You have development package of PostgreSQL installed or you built
   PostgreSQL from source.
 * You have flex and bison installed on your system. 
 * Your PATH variable is configured so that pg\_config command available, 
 or set PG_CONFIG variable.
    
Typical installation procedure may look like this:
    
    $ git clone https://github.com/nikitos94/monq.git
    $ cd monq
    $ make USE_PGXS=1
    $ sudo make USE_PGXS=1 install
    $ make USE_PGXS=1 installcheck
    $ psql DB -c "CREATE EXTENSION monq;"

### Usage

MonQ extension contains:
* `mquery` - datatype which represents MongoDB query in tree structure;
* `<=>` - maching operator which take like arguments jsonb document and 
mongoDB query. This operator have 2 variants of representation: 
`jsonb <=> mquery` and `mquery <=> jsonb`.

Example of query:

```
select '{ "a" : [ "ssl","security", "pattern"] }'::jsonb <=>
'{ a: { $all: [ "ssl","security"] } }'::mquery;
```

This mongoDB query:

```
{ a: { $all: [ "ssl","security"] } }
```

transformed to this JsQuery query:

```
a @> [ "ssl","security"]
```
and passed like arguments to JsQuery execution functions with jsonb document. 
Execution function return `true` or `false`dependently of result.

```
 ?column? 
----------
 t
(1 row)
```

You cah use key `a` without quotes, but if it complicated key `"a.b.c.qwerty.d"`
you need to use them. 


### MongoDB operators supported by MonQ

MonQ is limited by opportunities JsQuery language, but support main 
part of MongoDB query operators.

#### Comparison operators:
* `$eq` - supported;
* `$ne` - supported;
* `$lt` - supported;
* `$lte` - supported;
* `$gt` - supported;
* `$gte` - supported;
* `$in` - supported;
* `$nin` - supported.

#### Logical operators:
* `$and` - supported;
* `$or` - supported;
* `$not` - supported;
* `$nor` - supported.

#### Element operators:
* `$exists` - supported;
* `$type` - supported.

#### Evaluation operators:
* `$mod` - not supported;
* `$regex` - not supported;
* `$text` - supported;
* `$where` - not supported.

#### Bitwise operators:
* All operators is not supported.

#### Array operators:
* `$all` - supported;
* `$elemMatch` - supported;
* `$size` - supported.

#### Comment operators:
* All operators is not supported.

#### Geospatial operators:
* All operators is not supported.

#### Projextion operators:
* All operators is not supported.

Examples of queries with all this operators you can find in the file 
[sql/sql_test.sql](https://github.com/NikitOS94/MonQ/blob/master/sql/monq_test.sql)

### Contribution

Please, notice, that MonQ is still under development and while it's stable
and tested, it may contains some bugs. Don't hesitate to raise issues at 
github with your bug reports.

If you're lacking of some functionality in MonQ and feeling power to 
implement it then you're welcome to make pull requests.
