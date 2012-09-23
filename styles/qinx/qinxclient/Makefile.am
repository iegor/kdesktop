AUTOMAKE_OPTIONS = foreign

SUBDIRS = config

KDE_CXXFLAGS = -DQT_PLUGIN

INCLUDES = $(all_includes) -I$(kde_includes)/kwin

kwindir = $(kde_datadir)/kwin/
kwin_DATA = qinx.desktop

EXTRA_DIST = $(kwin_DATA)

noinst_HEADERS = kwinqinx.h bitmaps.h

kde_module_LTLIBRARIES = kwin3_qinx.la
kwin3_qinx_la_SOURCES = kwinqinx.cc
kwin3_qinx_la_LIBADD = $(kde_libraries)/libkdecorations.la
kwin3_qinx_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -lkdecore -module
kwin3_qinx_la_METASOURCES = AUTO

DISTCLEANFILES = $(kwin3_qinx_la_METASOURCES)

