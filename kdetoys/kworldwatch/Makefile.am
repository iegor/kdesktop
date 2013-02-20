SUBDIRS = maps pics

noinst_LTLIBRARIES = libww.la
libww_la_SOURCES = maploader.cpp mapwidget.cpp astro.c sunclock.c about.ui cities.cpp zoneclock.cpp flags.cpp flow.cpp clock.ui

KDE_ICON = kworldclock

bin_PROGRAMS = kworldclock

INCLUDES = $(all_includes)

kworldclock_SOURCES = main.cpp 
kworldclock_LDADD = libww.la $(LIB_KDEUI)
kworldclock_LDFLAGS = $(KDE_RPATH) $(all_libraries)

xdg_apps_DATA= kworldclock.desktop

progdir=$(kde_datadir)/kdesktop/programs
prog_DATA = kdeworld.desktop 

kde_module_LTLIBRARIES = ww_panelapplet.la

ww_panelapplet_la_SOURCES = applet.cpp

METASOURCES = main.moc zoneclock.moc applet.moc mapwidget.moc

noinst_HEADERS = applet.h

lnkdir = $(kde_datadir)/kicker/applets
lnk_DATA = kwwapplet.desktop 

tab_DATA = zone.tab
tabdir = $(kde_datadir)/kworldclock

EXTRA_DIST = $(lnk_DATA)

ww_panelapplet_la_LDFLAGS = $(KDE_PLUGIN) -module $(all_libraries)
ww_panelapplet_la_LIBADD = libww.la $(LIB_KSYCOCA) 

messages: rc.cpp
	$(XGETTEXT) rc.cpp $(kworldclock_SOURCES) $(libww_la_SOURCES) -o $(podir)/kworldclock.pot

