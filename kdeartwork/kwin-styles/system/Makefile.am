
INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = kwin3_system.la

kwin3_system_la_SOURCES = systemclient.cpp
kwin3_system_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
#kwin_system_la_LDFLAGS = $(all_libraries) -avoid-version -module $(KDE_RPATH) $(KDE_MT_LDFLAGS)
kwin3_system_la_LIBADD = $(LIB_KDEUI) -lkdecorations

METASOURCES = AUTO
noinst_HEADERS = systemclient.h

lnkdir = $(kde_datadir)/kwin/
lnk_DATA = system.desktop

EXTRA_DIST = $(lnk_DATA)

###KMAKE-start (don't edit or delete this block)

###KMAKE-end
