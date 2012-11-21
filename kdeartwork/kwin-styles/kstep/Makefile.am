
INCLUDES = $(all_includes) 

kde_module_LTLIBRARIES = kwin3_kstep.la

kwin3_kstep_la_SOURCES = nextclient.cpp
kwin3_kstep_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
#kwin_kstep_la_LDFLAGS = $(all_libraries) -avoid-version -module $(KDE_RPATH) $(KDE_MT_LDFLAGS)
kwin3_kstep_la_LIBADD = $(LIB_KDEUI) -lkdecorations

METASOURCES = AUTO
noinst_HEADERS = nextclient.h

lnkdir = $(kde_datadir)/kwin/
lnk_DATA = kstep.desktop

EXTRA_DIST = $(lnk_DATA)


###KMAKE-start (don't edit or delete this block)

###KMAKE-end
