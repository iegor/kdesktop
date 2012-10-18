lib_LTLIBRARIES = libkaffeineepgplugin.la

METASOURCES = AUTO

INCLUDES = -I$(top_srcdir)/kaffeine/src/input/dvb \
	-I$(top_srcdir)/kaffeine/src \
	$(all_includes)

kaffeineincludedir = $(includedir)/kaffeine
kaffeineinclude_HEADERS = kaffeinedvbevents.h kaffeinedvbsection.h

libkaffeineepgplugin_la_SOURCES = kaffeinedvbevents.cpp kaffeinedvbsection.cpp
libkaffeineepgplugin_la_LIBADD  = $(LIB_KPARTS)
libkaffeineepgplugin_la_LDFLAGS = $(all_libraries) -version-info 0:1:0 -no-undefined

kde_servicetypes_DATA = kaffeineepgplugin.desktop
