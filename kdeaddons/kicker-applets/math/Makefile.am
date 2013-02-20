INCLUDES = $(all_includes) 

kde_module_LTLIBRARIES = math_panelapplet.la

math_panelapplet_la_SOURCES = mathapplet.cpp parser.cpp

METASOURCES = mathapplet.moc
noinst_HEADERS = mathapplet.h parser.h

lnkdir = $(kde_datadir)/kicker/applets
lnk_DATA = mathapplet.desktop

EXTRA_DIST = $(lnk_DATA)

math_panelapplet_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
math_panelapplet_la_LIBADD = $(LIB_KSYCOCA) $(LIB_KDEUI)

messages:
	$(XGETTEXT) *.cpp *.h -o $(podir)/kmathapplet.pot
