
INCLUDES = -I$(srcdir)/../../common -I../../common $(all_includes)
kbabel_equationstool_la_LIBADD = $(LIB_KIO) ../../common/libkbabelcommon.la

####### Files

kde_module_LTLIBRARIES = kbabel_equationstool.la

kbabel_equationstool_la_SOURCES = main.cc

kbabel_equationstool_la_LDFLAGS = $(all_libraries) -avoid-version -module -no-undefined

noinst_HEADERS = main.h

kbabel_equationstool_la_METASOURCES = AUTO

service_DATA = kbabel_equationstool.desktop
servicedir = $(kde_servicesdir)

