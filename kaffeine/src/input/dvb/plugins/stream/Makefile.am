lib_LTLIBRARIES = libkaffeinedvbplugin.la

METASOURCES = AUTO

INCLUDES = -I$(top_srcdir)/kaffeine/src/input/dvb \
	-I$(top_srcdir)/kaffeine/src \
	$(all_includes)

kaffeineincludedir = $(includedir)/kaffeine
kaffeineinclude_HEADERS = kaffeinedvbplugin.h

libkaffeinedvbplugin_la_SOURCES = kaffeinedvbplugin.cpp
libkaffeinedvbplugin_la_LIBADD  = $(LIB_KPARTS)
libkaffeinedvbplugin_la_LDFLAGS = $(all_libraries) -version-info 0:1:0 -no-undefined

kde_servicetypes_DATA = kaffeinedvbplugin.desktop
