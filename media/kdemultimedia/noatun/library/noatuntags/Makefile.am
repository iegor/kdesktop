INCLUDES = -I$(top_srcdir)/noatun/library $(all_includes)

lib_LTLIBRARIES = libnoatuntags.la

libnoatuntags_la_SOURCES = tags.cpp
libnoatuntags_la_LDFLAGS = -version-info 3:0:2 $(all_libraries)
libnoatuntags_la_LIBADD = ../libnoatun.la

libnoatuntags_la_METASOURCES = AUTO

noatuntagsincludedir = $(includedir)/noatun
noatuntagsinclude_HEADERS = tags.h


