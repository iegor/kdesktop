INCLUDES= -I$(top_srcdir)/noatun/library $(all_includes)
kde_module_LTLIBRARIES = noatun_excellent.la

noatun_excellent_la_SOURCES = noatunui.cpp\
	userinterface.cpp 

noatun_excellent_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatun_excellent_la_LIBADD = $(LIB_KFILE) \
	$(top_builddir)/noatun/library/libnoatun.la \
	$(top_builddir)/noatun/library/libnoatuncontrols.la \
	-lqtmcop -lkmedia2_idl -lsoundserver_idl

noatun_excellent_la_METASOURCES = AUTO

noinst_HEADERS = userinterface.h

noatun_modules_excellent_DATA = excellent.plugin excellentui.rc
noatun_modules_excellentdir = $(kde_datadir)/noatun
