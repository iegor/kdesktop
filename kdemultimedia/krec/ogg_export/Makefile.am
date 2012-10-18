
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

kde_module_LTLIBRARIES = libkrecexport_ogg.la

libkrecexport_ogg_la_SOURCES = krecexport_ogg.cpp
libkrecexport_ogg_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) $(KDE_RPATH)
libkrecexport_ogg_la_LIBADD = ../lib_krec_common.la $(VORBISENC_LIBS) $(VORBIS_LIBS)

kde_services_DATA = krec_exportogg.desktop

