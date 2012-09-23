INCLUDES= -I$(top_srcdir)/noatun/library -I$(kde_includes)/arts $(all_includes)
kde_module_LTLIBRARIES = noatun_metatag.la

noatun_metatag_la_SOURCES = metatag.cpp edit.cpp

noatun_metatag_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatun_metatag_la_LIBADD = $(LIB_KIO) $(top_builddir)/noatun/library/libnoatun.la \
	$(top_builddir)/noatun/library/noatuntags/libnoatuntags.la

noatun_metatag_la_METASOURCES = AUTO

noinst_HEADERS = metatag.h edit.h

noatun_modules_metatag_DATA = metatag.plugin
noatun_modules_metatagdir = $(kde_datadir)/noatun
