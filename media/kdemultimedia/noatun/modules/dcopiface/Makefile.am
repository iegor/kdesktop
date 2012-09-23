INCLUDES= -I$(top_builddir)/noatun/library -I$(top_srcdir)/noatun/library -I$(kde_includes)/arts $(all_includes)
kde_module_LTLIBRARIES = noatun_dcopiface.la

noatun_dcopiface_la_SOURCES = dcopiface.cpp dcopiface.skel

noatun_dcopiface_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatun_dcopiface_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/library/libnoatun.la

noatun_dcopiface_la_METASOURCES = AUTO

noinst_HEADERS = dcopiface.h

noatun_modules_dcopiface_DATA = dcopiface.plugin
noatun_modules_dcopifacedir = $(kde_datadir)/noatun

dcopiface.lo: ../../library/noatunarts/noatunarts.h
