
INCLUDES = -I$(srcdir)/../../common -I../../common $(all_includes)
kbabel_argstool_la_LIBADD = $(LIB_KIO) ../../common/libkbabelcommon.la

####### Files

kde_module_LTLIBRARIES = kbabel_argstool.la

kbabel_argstool_la_SOURCES = main.cc

kbabel_argstool_la_LDFLAGS = $(all_libraries) -avoid-version -module -no-undefined

noinst_HEADERS = main.h

kbabel_argstool_la_METASOURCES = AUTO

service_DATA = kbabel_argstool.desktop
servicedir = $(kde_servicesdir)

