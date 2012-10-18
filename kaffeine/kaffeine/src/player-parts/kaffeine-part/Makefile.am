lib_LTLIBRARIES = libkaffeinepart.la

INCLUDES = $(all_includes)

kaffeineincludedir = $(includedir)/kaffeine
kaffeineinclude_HEADERS = kaffeinepart.h mrl.h

METASOURCES = AUTO

libkaffeinepart_la_SOURCES = kaffeinepart.cpp mrl.cpp playlistimport.cpp
libkaffeinepart_la_LIBADD  = $(LIB_KPARTS) $(all_libraries)
libkaffeinepart_la_LDFLAGS = -no-undefined -avoid-version
