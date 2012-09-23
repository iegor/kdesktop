noinst_LTLIBRARIES = libkmrmlstuff.la
libkmrmlstuff_la_SOURCES = kmrml_config.cpp mrml_shared.cpp mrml_utils.cpp\
watcher_stub.cpp
noinst_HEADERS = kmrml_config.h mrml_shared.h mrml_utils.h watcher_stub.h

METASOURCES = AUTO

libkmrmlstuff_la_LDFLAGS = $(all_libraries) -no-undefined
libkmrmlstuff_la_LIBADD = $(LIB_KDECORE)

INCLUDES = -I$(top_srcdir) $(all_includes)
