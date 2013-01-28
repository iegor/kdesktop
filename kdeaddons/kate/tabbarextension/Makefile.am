INCLUDES    = $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = libkatetabbarextensionplugin.la

libkatetabbarextensionplugin_la_SOURCES = plugin_katetabbarextension.cpp
libkatetabbarextensionplugin_la_LIBADD = -lkateinterfaces
libkatetabbarextensionplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

pluginsdir = $(kde_datadir)/kate/plugins/katetabbarextension
plugins_DATA = ui.rc

kde_services_DATA = katetabbarextension.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/katetabbarextension.pot
