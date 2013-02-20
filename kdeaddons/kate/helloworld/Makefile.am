INCLUDES    =  $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = katehelloworldplugin.la

katehelloworldplugin_la_SOURCES = plugin_katehelloworld.cpp
katehelloworldplugin_la_LIBADD = -lkateinterfaces
katehelloworldplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

pluginsdir = $(kde_datadir)/kate/plugins/katehelloworld
plugins_DATA = ui.rc

kde_services_DATA = katehelloworld.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/katehelloworld.pot

