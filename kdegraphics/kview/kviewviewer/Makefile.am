SUBDIRS = test config

kde_module_LTLIBRARIES = libkviewviewer.la
INCLUDES = -I$(top_srcdir)/kview $(all_includes)

noinst_HEADERS = kviewkonqextension.h kviewviewer.h

libkviewviewer_la_SOURCES = printimagesettings.ui imagesettings.cpp kviewkonqextension.cpp kviewviewer.cpp
libkviewviewer_la_LDFLAGS = $(all_libraries) -avoid-version -module -no-undefined
libkviewviewer_la_LIBADD = $(LIB_KDEPRINT) $(LIB_KUTILS) \
                           $(top_builddir)/kview/kimageviewer/libkimageviewer.la

METASOURCES = AUTO

kde_services_DATA = kviewviewer.desktop

partdir = $(kde_datadir)/kviewviewer
part_DATA = kviewviewer.rc kviewviewer_ro.rc kviewpopup.rc

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kviewviewer.pot
