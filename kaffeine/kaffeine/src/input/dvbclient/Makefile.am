INCLUDES = -I$(top_srcdir)/kaffeine/src/input/ \
	$(all_includes)

METASOURCES = AUTO

noinst_LTLIBRARIES = libkaffeinedvbclient.la

libkaffeinedvbclient_la_SOURCES = cdchannel.cpp \
	cdchannel.h \
	cdwidget.cpp \
	cdwidget.h \
	cddump.cpp \
	cddump.h \
	cdlisten.cpp \
	cdlisten.h \
	cdcleaner.cpp \
	cdcleaner.h

libkaffeinedvbclient_la_LDFLAGS = $(KDE_RPATH) \
	$(all_libraries) \
	-L$(top_srcdir)/kaffeine/src/input

libkaffeinedvbclient_la_LIBADD   = $(top_builddir)/kaffeine/src/input/libkaffeineinput.la
