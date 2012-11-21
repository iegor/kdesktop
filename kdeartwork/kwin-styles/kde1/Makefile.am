
INCLUDES = $(all_includes) 

kde_module_LTLIBRARIES = kwin3_kde1.la

kwin3_kde1_la_SOURCES = kde1client.cpp
kwin3_kde1_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kwin3_kde1_la_LIBADD  = -lkdecorations

METASOURCES = AUTO
noinst_HEADERS = kde1client.h

lnkdir = $(kde_datadir)/kwin/
lnk_DATA = kde1.desktop
EXTRA_DIST = $(lnk_DATA)

