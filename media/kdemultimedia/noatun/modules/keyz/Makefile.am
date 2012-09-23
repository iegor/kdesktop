INCLUDES= -I$(top_srcdir)/noatun/library  -I$(kde_includes)/arts $(all_includes)
kde_module_LTLIBRARIES = noatun_keyz.la

noatun_keyz_la_SOURCES = keyz.cpp

noatun_keyz_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatun_keyz_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/library/libnoatun.la

noatun_keyz_la_METASOURCES = AUTO

noinst_HEADERS = keyz.h

noatun_modules_keyz_DATA = keyz.plugin
noatun_modules_keyzdir = $(kde_datadir)/noatun
