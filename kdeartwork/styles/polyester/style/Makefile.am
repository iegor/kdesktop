AUTOMAKE_OPTIONS = foreign

SUBDIRS = config

KDE_CXXFLAGS = -DQT_PLUGIN

METASOURCES = AUTO

INCLUDES = $(all_includes)

stylercdir = $(kde_datadir)/kstyle/themes
stylerc_DATA = polyester.themerc

EXTRA_DIST = $(stylerc_DATA)

noinst_HEADERS = polyester.h misc.h

kde_style_LTLIBRARIES = polyester.la
polyester_la_SOURCES = polyester.cpp misc.cpp
polyester_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
polyester_la_LIBADD = -lkdefx
