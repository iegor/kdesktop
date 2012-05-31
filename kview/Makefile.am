SUBDIRS = kimageviewer . config kviewcanvas kviewviewer modules photobook

INCLUDES = -I$(top_srcdir)/kview $(all_includes)

METASOURCES = AUTO

bin_PROGRAMS =
lib_LTLIBRARIES =
kdeinit_LTLIBRARIES = kview.la

noinst_HEADERS = kview.h

kview_la_SOURCES = kview.cpp main.cpp
kview_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) -module -avoid-version
kview_la_LIBADD = $(LIB_KPARTS) $(LIB_KUTILS)

xdg_apps_DATA = kview.desktop

KDE_ICON = kview

rcdir = $(kde_datadir)/kview
rc_DATA = kviewui.rc

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kview.pot
