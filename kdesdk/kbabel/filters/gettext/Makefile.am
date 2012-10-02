####### General stuff

INCLUDES= -I../../common -I$(srcdir)/../../common $(all_includes)

kde_module_LTLIBRARIES = kbabel_gettextimport.la kbabel_gettextexport.la

kbabel_gettextimport_la_SOURCES = gettextimport.cpp
kbabel_gettextimport_la_LIBADD = ../../common/libkbabelcommon.la
kbabel_gettextimport_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

kbabel_gettextexport_la_SOURCES = gettextexport.cpp
kbabel_gettextexport_la_LIBADD = ../../common/libkbabelcommon.la
kbabel_gettextexport_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

METASOURCES = AUTO

service_DATA = kbabel_gettext_import.desktop kbabel_gettext_export.desktop
servicedir = $(kde_servicesdir)

gettextexport.lo: ../../common/kbprojectsettings.h
