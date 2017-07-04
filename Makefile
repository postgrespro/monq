MODULE_big = monq
OBJS = monq_gram.o monq_scan.o get_jsquery.o create_mquery.o jsquery_calls.o delete_mquery.o
INCLUDES = monq_structures.h
EXTENSION = monq        	# the extensions name
DATA = monq--0.1.sql  		# script files to install
REGRESS = monq_test     	# our test script file (without extension)
MODULES = monq          	# our c module file to build
ENCODING = UTF8

EXTRA_CLEAN = get_jsquery.o monq_gram.o monq_scan.o create_mquery.o jsquery_calls.o monq_scan.c monq_gram.c monq_gram.h

# postgres build stuff
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

monq_gram.o: monq_scan.c

monq_gram.c: BISONFLAGS += -dt

distprep: monq_gram.c monq_scan.c
