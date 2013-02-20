AUTOMAKE_OPTIONS = foreign

SUBDIRS = config

KDE_CXXFLAGS = -DQT_PLUGIN

INCLUDES = $(all_includes) -I$(kde_includes)/kwin

kwindir = $(kde_datadir)/kwin/
kwin_DATA = smoothblend.desktop

noinst_HEADERS = smoothblend.h buttons.h

kde_module_LTLIBRARIES = kwin3_smoothblend.la
kwin3_smoothblend_la_SOURCES = smoothblend.cc
kwin3_smoothblend_la_LIBADD = -lkdecorations
kwin3_smoothblend_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -lkdecore -module
kwin3_smoothblend_la_METASOURCES = AUTO

DISTCLEANFILES = $(kwin3_smoothblend_la_METASOURCES)
