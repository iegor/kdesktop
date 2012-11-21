INCLUDES = $(all_includes) 

kde_module_LTLIBRARIES = fifteen_panelapplet.la

fifteen_panelapplet_la_SOURCES = fifteenapplet.cpp qttableview.cpp

METASOURCES = AUTO
noinst_HEADERS = fifteenapplet.h

lnkdir = $(kde_datadir)/kicker/applets
lnk_DATA = kfifteenapplet.desktop

EXTRA_DIST = $(lnk_DATA)

fifteen_panelapplet_la_LDFLAGS = $(KDE_PLUGIN) -module $(all_libraries)
fifteen_panelapplet_la_LIBADD = $(LIB_KDEUI)

messages:
	$(XGETTEXT) *.cpp *.h -o $(podir)/kfifteenapplet.pot
