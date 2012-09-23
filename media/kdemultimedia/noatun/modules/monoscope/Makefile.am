INCLUDES= -I$(top_srcdir)/noatun/library $(all_includes)
kde_module_LTLIBRARIES = noatun_monoscope.la

noatun_monoscope_la_SOURCES = monoscope.cpp 
#prefs.cpp

noatun_monoscope_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatun_monoscope_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/library/libnoatun.la -lm

noatun_monoscope_la_METASOURCES = AUTO

noinst_HEADERS = monoscope.h
#prefs.h

noatun_modules_monoscope_DATA = monoscope.plugin
noatun_modules_monoscopedir = $(kde_datadir)/noatun
