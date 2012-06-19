####### General stuff

INCLUDES = -I../../common -I$(srcdir)/../../common $(all_includes)

kde_module_LTLIBRARIES = kbabel_linguistexport.la kbabel_linguistimport.la

kbabel_linguistexport_la_SOURCES = linguistexport.cpp
kbabel_linguistexport_la_LIBADD = ../../common/libkbabelcommon.la
kbabel_linguistexport_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

kbabel_linguistimport_la_SOURCES = linguistimport.cpp
kbabel_linguistimport_la_LIBADD = ../../common/libkbabelcommon.la
kbabel_linguistimport_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

METASOURCES = AUTO

service_DATA = kbabel_linguist_export.desktop kbabel_linguist_import.desktop
servicedir = $(kde_servicesdir)
