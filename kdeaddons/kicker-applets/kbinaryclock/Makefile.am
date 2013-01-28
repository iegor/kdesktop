INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = kbinaryclock_panelapplet.la

kbinaryclock_panelapplet_la_SOURCES = kbinaryclock.cpp settings.ui datepicker.cpp prefs.kcfgc

METASOURCES = AUTO

noinst_HEADERS = kbinaryclock.h datepicker.h

lnkdir = $(kde_datadir)/kicker/applets
lnk_DATA = kbinaryclock.desktop
kde_kcfg_DATA = kbinaryclock.kcfg

EXTRA_DIST = $(lnk_DATA)

kbinaryclock_panelapplet_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
kbinaryclock_panelapplet_la_LIBADD = $(LIB_KDEUI)

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kbinaryclock.pot

