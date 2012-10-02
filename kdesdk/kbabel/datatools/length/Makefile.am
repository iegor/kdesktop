
INCLUDES = -I$(srcdir)/../../common -I../../common $(all_includes)
kbabel_lengthtool_la_LIBADD = $(LIB_KIO) ../../common/libkbabelcommon.la

####### Files

kde_module_LTLIBRARIES = kbabel_lengthtool.la

kbabel_lengthtool_la_SOURCES = main.cc

kbabel_lengthtool_la_LDFLAGS = $(all_libraries) -avoid-version -module -no-undefined

noinst_HEADERS = main.h

kbabel_lengthtool_la_METASOURCES = AUTO

service_DATA = kbabel_lengthtool.desktop
servicedir = $(kde_servicesdir)

