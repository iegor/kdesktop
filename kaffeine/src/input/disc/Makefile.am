METASOURCES = AUTO

SUBDIRS = plugins .

noinst_LTLIBRARIES = libkaffeinedisc.la

noinst_HEADERS = cddb.h \
	disc.h \
	paranoia.h

INCLUDES = -I$(top_srcdir)/kaffeine/src/input/ \
	-I$(top_builddir)/kaffeine/src/input/ \
	-I$(top_srcdir)/kaffeine/src/ \
	-I$(top_srcdir)/kaffeine/src/player-parts/kaffeine-part \
	-I$(top_srcdir)/kaffeine/src/input/disc/plugins \
	$(all_includes) 

libkaffeinedisc_la_SOURCES = disc.cpp \
	cddb.cpp \
	paranoia.cpp \
	paranoiasettings.ui

libkaffeinedisc_la_LDFLAGS = $(KDE_RPATH) \
	$(all_libraries) \
	-L$(top_srcdir)/kaffeine/src/input \
	-L$(top_srcdir)/kaffeine/src/input/disc/plugins

libkaffeinedisc_la_LIBADD = $(top_builddir)/kaffeine/src/input/libkaffeineinput.la \
	$(top_builddir)/kaffeine/src/player-parts/kaffeine-part/libkaffeinepart.la \
	$(top_builddir)/kaffeine/src/input/disc/plugins/libkaffeineaudioencoder.la \
	-lcdda_interface -lcdda_paranoia

# this is where the XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/kaffeine
shellrc_DATA = kaffeinedisc.rc

