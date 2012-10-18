
INCLUDES= -I$(top_srcdir)/noatun/library $(all_includes)
kde_module_LTLIBRARIES = noatun_htmlexport.la

noatun_htmlexport_la_SOURCES = htmlexport.cpp
noinst_HEADERS = htmlexport.h

noatun_htmlexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatun_htmlexport_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/library/libnoatun.la \
                        -lqtmcop -lkmedia2_idl -lsoundserver_idl

noatun_htmlexport_la_METASOURCES = AUTO


noatun_modules_htmlexport_DATA = htmlexport.plugin
noatun_modules_htmlexportdir = $(kde_datadir)/noatun
