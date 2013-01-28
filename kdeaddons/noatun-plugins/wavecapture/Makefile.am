# $Id: Makefile.am 222514 2003-04-28 06:57:43Z coolo $

INCLUDES = -I$(kde_includes)/arts $(all_includes)
kde_module_LTLIBRARIES = noatunwavecapture.la


noatunwavecapture_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatunwavecapture_la_LIBADD = $(LIB_KFILE) -lnoatun -lm -lartsmodules

noatunwavecapture_la_SOURCES = wavecapture.cpp
noatunwavecapture_la_METASOURCES = AUTO
noinst_HEADERS = wavecapture.h

noatun_DATA = wavecapture.plugin
noatundir = $(kde_datadir)/noatun

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/wavecapture.pot
