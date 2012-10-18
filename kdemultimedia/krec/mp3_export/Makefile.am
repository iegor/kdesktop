
INCLUDES= \
	-I$(top_builddir)/arts/gui/common \
	-I$(top_builddir)/arts/midi \
	-I$(top_builddir)/arts/modules/common \
	-I$(top_builddir)/arts/modules/synth \
	-I$(top_builddir)/arts/modules/effects \
	-I$(top_srcdir)/arts/gui/kde \
	-I$(top_srcdir)/arts/tools \
	-I$(top_srcdir)/krec \
	-I$(arts_includes) $(all_includes)

METASOURCES = AUTO

kde_module_LTLIBRARIES = libkrecexport_mp3.la

libkrecexport_mp3_la_SOURCES = krecexport_mp3.cpp
libkrecexport_mp3_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) $(KDE_RPATH)
libkrecexport_mp3_la_LIBADD = ../lib_krec_common.la $(LAME_LIBS)

kde_services_DATA = krec_exportmp3.desktop

