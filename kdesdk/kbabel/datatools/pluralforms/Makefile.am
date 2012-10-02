
INCLUDES = -I$(srcdir)/../../common -I../../common $(all_includes)
kbabel_pluraltool_la_LIBADD = $(LIB_KIO) ../../common/libkbabelcommon.la

####### Files

kde_module_LTLIBRARIES = kbabel_pluraltool.la

kbabel_pluraltool_la_SOURCES = main.cc

kbabel_pluraltool_la_LDFLAGS = $(all_libraries) -avoid-version -module -no-undefined

noinst_HEADERS = main.h

kbabel_pluraltool_la_METASOURCES = AUTO

service_DATA = kbabel_pluralformstool.desktop
servicedir = $(kde_servicesdir)

