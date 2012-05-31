INCLUDES= -I$(top_srcdir)/noatun/library $(all_includes)
kde_module_LTLIBRARIES = noatun_marquis.la

noatun_marquis_la_SOURCES = marquis.cpp plugin.cpp

noatun_marquis_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatun_marquis_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/library/libnoatun.la \
                        -lqtmcop -lkmedia2_idl -lsoundserver_idl

noatun_marquis_la_METASOURCES = AUTO

noinst_HEADERS = marquis.h

noatun_modules_marquis_DATA = marquis.plugin
noatun_modules_marquisdir = $(kde_datadir)/noatun
