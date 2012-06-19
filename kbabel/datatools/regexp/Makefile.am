
INCLUDES = -I$(srcdir)/../../common -I../../common $(all_includes)
kbabel_regexptool_la_LIBADD = $(LIB_KIO) ../../common/libkbabelcommon.la

####### Files

kde_module_LTLIBRARIES = kbabel_regexptool.la

kbabel_regexptool_la_SOURCES = main.cc

kbabel_regexptool_la_LDFLAGS = $(all_libraries) -avoid-version -module -no-undefined

noinst_HEADERS = main.h

kbabel_regexptool_la_METASOURCES = AUTO

service_DATA = kbabel_regexptool.desktop
servicedir = $(kde_servicesdir)

regexp_DATA = regexplist.xml
regexpdir = $(kde_datadir)/kbabel
