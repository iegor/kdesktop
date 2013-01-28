INCLUDES = $(all_includes)
METASOURCES = AUTO
kde_module_LTLIBRARIES = librellinksplugin.la
librellinksplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
librellinksplugin_la_LIBADD = $(LIB_KPARTS) $(LIB_KHTML)
librellinksplugin_la_SOURCES = plugin_rellinks.cpp
plugins_DATA = plugin_rellinks.rc plugin_rellinks.desktop
pluginsdir = $(kde_datadir)/khtml/kpartplugins

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/rellinks.pot

