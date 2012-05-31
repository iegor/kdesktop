INCLUDES = -I$(top_srcdir)/noatun/library -I$(kde_includes)/arts $(all_includes)

kdeinit_LTLIBRARIES = noatun.la
noatun_la_SOURCES = main.cpp
noatun_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) -module -avoid-version
noatun_la_LIBADD = -lqtmcop -lkmedia2_idl -lsoundserver_idl $(top_builddir)/noatun/library/libnoatun.la

bin_PROGRAMS =
lib_LTLIBRARIES =

METASOURCES = AUTO
