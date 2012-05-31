SUBDIRS = skins
INCLUDES= -I$(top_srcdir)/noatun/library $(all_includes)
kde_module_LTLIBRARIES = noatun_kaiman.la

noatun_kaiman_la_SOURCES = \
	noatunui.cpp \
	style.cpp \
	userinterface.cpp \
	pref.cpp

noatun_kaiman_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatun_kaiman_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/library/libnoatun.la \
                        -lqtmcop -lkmedia2_idl -lsoundserver_idl

noatun_kaiman_la_METASOURCES = AUTO

noinst_HEADERS = \
	userinterface.h \
	style.h \
	pref.h

noatun_modules_kaiman_DATA = kaiman.plugin kaimanui.rc
noatun_modules_kaimandir = $(kde_datadir)/noatun
