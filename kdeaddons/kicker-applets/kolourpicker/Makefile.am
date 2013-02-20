
INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = kolourpicker_panelapplet.la

kolourpicker_panelapplet_la_SOURCES = kolourpicker.cpp simplebutton.cpp
                          
noinst_HEADERS = kolourpicker.h

kolourpicker_panelapplet_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
kolourpicker_panelapplet_la_LIBADD = $(LIB_KDEUI)

METASOURCES = AUTO

lnkdir = $(kde_datadir)/kicker/applets

lnk_DATA = kolourpicker.desktop 

EXTRA_DIST = $(lnk_DATA)

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kolourpicker.pot

