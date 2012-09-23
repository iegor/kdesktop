SUBDIRS = config test

kde_module_LTLIBRARIES = libkviewcanvas.la
INCLUDES = -I$(top_srcdir)/kview $(all_includes)

noinst_HEADERS = kimageholder.h kimagecanvas.h

libkviewcanvas_la_SOURCES = kimageholder.cpp kimagecanvas.cpp
libkviewcanvas_la_LDFLAGS = $(all_libraries) -module -no-undefined -avoid-version
libkviewcanvas_la_LIBADD = $(LIB_KPARTS) $(LIB_KDEPRINT) $(LIB_KUTILS) \
                           $(top_builddir)/kview/kimageviewer/libkimageviewer.la

METASOURCES = AUTO

kde_services_DATA = kviewcanvas.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kviewcanvas.pot
