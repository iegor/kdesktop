
INCLUDES = $(all_includes)

SUBDIRS = . config themes

kde_module_LTLIBRARIES = kwin3_glow.la

kwin3_glow_la_SOURCES = glowclient.cpp glowbutton.cpp
kwin3_glow_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kwin3_glow_la_LIBADD  = $(LIB_KDECORE) -lkdecorations

METASOURCES = AUTO
noinst_HEADERS = glowclient.h glowbutton.h resources.h

linkdir = $(kde_datadir)/kwin/
link_DATA = glow.desktop

EXTRA_DIST = $(link_DATA)

###KMAKE-start (don't edit or delete this block)

###KMAKE-end
