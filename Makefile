EXTENSION   = ptree
EXTVERSION  = $(shell grep default_version $(EXTENSION).control | sed -e "s/default_version[[:space:]]*=[[:space:]]*'\([^']*\)'/\1/")
MODULE_big  = $(EXTENSION)
REGRESS     = $(EXTENSION)
OBJS        = $(patsubst %.c, %.o, $(wildcard src/*.c))
DATA        = sql/$(EXTENSION)--$(EXTVERSION).sql sql/$(EXTENSION)--unpackaged--$(EXTVERSION).sql

PG_CPPFLAGS = -DLOWER_NODE -msse4.2 -march=native

PG_CONFIG   = pg_config
PGXS        := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)