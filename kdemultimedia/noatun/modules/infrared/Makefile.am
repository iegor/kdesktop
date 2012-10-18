
INCLUDES= -I$(top_srcdir)/noatun/library $(all_includes)
kde_module_LTLIBRARIES = noatun_infrared.la

noatun_infrared_la_SOURCES = infrared.cpp lirc.cpp irprefs.cpp
noinst_HEADERS = infrared.h lirc.h irprefs.h

noatun_infrared_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatun_infrared_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/library/libnoatun.la \
                        -lqtmcop -lkmedia2_idl -lsoundserver_idl

noatun_infrared_la_METASOURCES = AUTO


noatun_modules_infrared_DATA = infrared.plugin
noatun_modules_infrareddir = $(kde_datadir)/noatun
