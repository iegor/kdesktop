AUTOMAKE_OPTIONS = foreign

SUBDIRS = config
KDE_CXXFLAGS = -DQT_PLUGIN

INCLUDES = $(all_includes)

styledir = $(kde_datadir)/kstyle/themes
style_DATA = qinx.themerc

colordir = $(kde_datadir)/kdisplay/color-schemes
color_DATA = qinx.kcsrc qinxmodern.kcsrc

EXTRA_DIST = $(style_DATA) $(color_DATA)

noinst_HEADERS = qinxstyle.h

kde_style_LTLIBRARIES = qinx.la
qinx_la_SOURCES = qinxstyle.cc
qinx_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -lkdefx -lkdecore -module
qinx_la_METASOURCES = AUTO

DISTCLEANFILES = $(qinx_la_METASOURCES)
