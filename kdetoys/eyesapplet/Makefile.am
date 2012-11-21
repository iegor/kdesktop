INCLUDES = $(all_includes) 

kde_module_LTLIBRARIES = eyes_panelapplet.la

eyes_panelapplet_la_SOURCES = eyes.cpp

METASOURCES = AUTO
noinst_HEADERS = eyes.h

lnkdir = $(kde_datadir)/kicker/applets
lnk_DATA = eyesapplet.desktop

EXTRA_DIST = $(lnk_DATA)

eyes_panelapplet_la_LDFLAGS = $(KDE_PLUGIN) -module $(all_libraries)
eyes_panelapplet_la_LIBADD = $(LIB_KDEUI)

messages:
	$(XGETTEXT) *.cpp *.h -o $(podir)/keyesapplet.pot

