kde_module_LTLIBRARIES = ktimemon_panelapplet.la

ktimemon_panelapplet_la_SOURCES = confdlg.cc sample.cc timemon.cc
METASOURCES = AUTO

noinst_HEADERS = confdlg.h sample.h timemon.h

INCLUDES= $(all_includes)

ktimemon_panelapplet_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
ktimemon_panelapplet_la_LIBADD   = $(LIB_KDEUI) $(LIBKSTAT) $(LIB_KIO)

KDE_ICON = ktimemon

lnk_DATA = ktimemon.desktop
lnkdir   = $(kde_datadir)/kicker/applets

EXTRA_DIST = $(lnk_DATA)

messages:
	$(XGETTEXT) *.cc -o $(podir)/ktimemon.pot

