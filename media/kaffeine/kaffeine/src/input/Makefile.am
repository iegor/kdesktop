noinst_LTLIBRARIES = libkaffeineinput.la

if with_dvb
    DVB_SUBDIR = dvb
endif

SUBDIRS = . dvbclient audiobrowser disc $(DVB_SUBDIR)

INCLUDES = -I$(top_srcdir)/kaffeine/src \
	   -I$(top_srcdir)/kaffeine/src/player-parts/kaffeine-part $(all_includes)

noinst_HEADERS =  kaffeineinput.h

METASOURCES = AUTO

libkaffeineinput_la_SOURCES = kaffeineinput.cpp

libkaffeineinput_la_LDFLAGS = $(KDE_RPATH) \
	$(all_libraries)

libkaffeineinput_la_LIBADD = $(top_builddir)/kaffeine/src/player-parts/kaffeine-part/libkaffeinepart.la
