INCLUDES= -I$(top_srcdir)/noatun/library $(all_includes)
kde_module_LTLIBRARIES = noatun_net.la

noatun_net_la_SOURCES = net.cpp

noatun_net_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatun_net_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/library/libnoatun.la

noatun_net_la_METASOURCES = AUTO

noinst_HEADERS = net.h

noatun_modules_net_DATA = net.plugin
noatun_modules_netdir = $(kde_datadir)/noatun
