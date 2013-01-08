AUTOMAKE_OPTIONS = foreign

SUBDIRS = pics

KDE_CXXFLAGS = -DQT_PLUGIN

INCLUDES = $(all_includes) -I$(kde_includes)/kwin

kwindir = $(kde_datadir)/kwin/
kwin_DATA = fahrenheit.desktop

noinst_HEADERS = fahrenheitclient.h

kde_module_LTLIBRARIES = kwin3_fahrenheit.la
kwin3_fahrenheit_la_SOURCES = fahrenheitclient.cc
kwin3_fahrenheit_la_LIBADD = $(kde_libraries)/libkdecorations.la
kwin3_fahrenheit_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -lkdecore -module
kwin3_fahrenheit_la_METASOURCES = AUTO

DISTCLEANFILES = $(kwin3_fahrenheit_la_METASOURCES)
