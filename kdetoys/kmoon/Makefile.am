
kde_module_LTLIBRARIES = kmoon_panelapplet.la

kmoon_panelapplet_la_SOURCES = kmoonapplet.cpp phases.cpp kmoondlg.cpp kmoonwidget.cpp
kmoon_panelapplet_la_LDFLAGS = $(KDE_PLUGIN) -module $(all_libraries)
kmoon_panelapplet_la_LIBADD = $(LIB_KSYCOCA)

lnkdir = $(kde_datadir)/kicker/applets
lnk_DATA = kmoonapplet.desktop

INCLUDES = $(all_includes)

METASOURCES = AUTO

KDE_ICON = kmoon

SUBDIRS = pics

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kmoon.pot
