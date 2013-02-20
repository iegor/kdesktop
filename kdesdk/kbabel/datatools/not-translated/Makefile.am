
INCLUDES = -I$(srcdir)/../../common -I../../common $(all_includes)
kbabel_nottranslatedtool_la_LIBADD = $(LIB_KIO) ../../common/libkbabelcommon.la

####### Files

kde_module_LTLIBRARIES = kbabel_nottranslatedtool.la

kbabel_nottranslatedtool_la_SOURCES = main.cc

kbabel_nottranslatedtool_la_LDFLAGS = $(all_libraries) -avoid-version -module -no-undefined

noinst_HEADERS = main.h

kbabel_nottranslatedtool_la_METASOURCES = AUTO

service_DATA = kbabel_nottranslatedtool.desktop
servicedir = $(kde_servicesdir)

