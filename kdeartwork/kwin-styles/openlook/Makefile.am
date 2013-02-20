INCLUDES                    = $(all_includes)

kde_module_LTLIBRARIES      = kwin3_openlook.la

kwin3_openlook_la_SOURCES   = OpenLook.cpp
kwin3_openlook_la_LDFLAGS   = $(all_libraries) $(KDE_PLUGIN) -module
kwin3_openlook_la_LIBADD    = -lkdecorations

noinst_HEADERS              = OpenLook.h
METASOURCES                 = AUTO

linkdir                     = $(kde_datadir)/kwin/
link_DATA                   = openlook.desktop

EXTRA_DIST                  = $(link_DATA)

