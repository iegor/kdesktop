INCLUDES    = $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = katesnippetsplugin.la

katesnippetsplugin_la_SOURCES = plugin_katesnippets.cpp CWidgetSnippetsBase.ui cwidgetsnippets.h cwidgetsnippets.cpp csnippet.h csnippet.cpp CWidgetSnippetsBase.ui.h
katesnippetsplugin_la_LIBADD = -lkateinterfaces
katesnippetsplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

pluginsdir = $(kde_datadir)/kate/plugins/katesnippets
plugins_DATA = plugin_katesnippets.rc

kde_services_DATA = katesnippets.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/katesnippets.pot
