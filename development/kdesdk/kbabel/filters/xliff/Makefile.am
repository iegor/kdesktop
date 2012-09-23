####### General stuff

INCLUDES = -I../../common -I$(srcdir)/../../common $(all_includes)

kde_module_LTLIBRARIES = kbabel_xliffexport.la kbabel_xliffimport.la

kbabel_xliffexport_la_SOURCES = xliffexport.cpp
kbabel_xliffexport_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
kbabel_xliffexport_la_LIBADD = ../../common/libkbabelcommon.la

kbabel_xliffimport_la_SOURCES = xliffimport.cpp
kbabel_xliffimport_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
kbabel_xliffimport_la_LIBADD = ../../common/libkbabelcommon.la

METASOURCES = AUTO

service_DATA = kbabel_xliff_export.desktop kbabel_xliff_import.desktop
servicedir = $(kde_servicesdir)
