AUTOMAKE_OPTIONS = foreign

SUBDIRS = config

KDE_CXXFLAGS = -DQT_PLUGIN

INCLUDES = $(all_includes)

styledir = $(kde_datadir)/kstyle/themes
style_DATA = phase.themerc

EXTRA_DIST = $(style_DATA)

noinst_HEADERS = phasestyle.h bitmaps.h

kde_style_LTLIBRARIES = phasestyle.la
phasestyle_la_SOURCES = phasestyle.cpp
phasestyle_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
phasestyle_la_LIBADD = -lkdefx -lkdecore
phasestyle_la_METASOURCES = AUTO

DISTCLEANFILES = $(phasestyle_la_METASOURCES)
