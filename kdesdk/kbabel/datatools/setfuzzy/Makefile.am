
INCLUDES = -I$(srcdir)/../../common -I../../common $(all_includes)
kbabel_setfuzzytool_la_LIBADD = ../../common/libkbabelcommon.la

####### Files

kde_module_LTLIBRARIES = kbabel_setfuzzytool.la

kbabel_setfuzzytool_la_SOURCES = main.cc

kbabel_setfuzzytool_la_LDFLAGS = $(all_libraries) -avoid-version -module -no-undefined

noinst_HEADERS = main.h

kbabel_setfuzzytool_la_METASOURCES = AUTO

service_DATA = kbabel_setfuzzytool.desktop
servicedir = $(kde_servicesdir)

