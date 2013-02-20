INCLUDES= -I$(kde_includes)/arts $(all_includes)
kde_module_LTLIBRARIES = noatun_ffrs.la

noatun_ffrs_la_SOURCES = ffrs.cpp

noatun_ffrs_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatun_ffrs_la_LIBADD = $(LIB_KFILE) -lnoatun -lm

noatun_ffrs_la_METASOURCES = AUTO

noinst_HEADERS = ffrs.h

noatun_DATA = ffrs.plugin
noatundir = $(kde_datadir)/noatun

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/ffrs.pot
