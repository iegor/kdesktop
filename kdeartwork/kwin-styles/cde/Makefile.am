
INCLUDES = $(all_includes) 

SUBDIRS = . config

kde_module_LTLIBRARIES = kwin3_cde.la

kwin3_cde_la_SOURCES = cdeclient.cpp
kwin3_cde_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kwin3_cde_la_LIBADD = -lkdecorations

METASOURCES = AUTO
noinst_HEADERS = cdeclient.h

lnkdir = $(kde_datadir)/kwin/
lnk_DATA = cde.desktop

EXTRA_DIST = $(lnk_DATA)

###KMAKE-start (don't edit or delete this block)

###KMAKE-end
