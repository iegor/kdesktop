AUTOMAKE_OPTIONS = foreign

SUBDIRS = config images

KDE_CXXFLAGS = -DQT_PLUGIN

METASOURCES = AUTO

INCLUDES = $(all_includes) -I$(kde_includes)/kwin

kwindir = $(kde_datadir)/kwin/
kwin_DATA = polyester.desktop

noinst_HEADERS = polyester.h buttons.h shadow.h ../style/misc.h

kde_module_LTLIBRARIES = kwin3_polyester.la
kwin3_polyester_la_SOURCES = polyester.cc shadow.cpp ../style/misc.cpp
kwin3_polyester_la_LIBADD = $(kde_libraries)/libkdecorations.la
kwin3_polyester_la_LDFLAGS = -module $(all_libraries) $(KDE_PLUGIN) -lkdecore
