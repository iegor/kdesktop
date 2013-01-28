INCLUDES    = $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = katetextfilterplugin.la

# This is all standard.  Remove the LIB_KHTML reference if you are not
# using the KHTML Part
katetextfilterplugin_la_SOURCES = plugin_katetextfilter.cpp
katetextfilterplugin_la_LIBADD = -lkateinterfaces
katetextfilterplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# Install the .rc file in the Part's directory (in this case, the part
# is KHTMLPart)
pluginsdir = $(kde_datadir)/kate/plugins/katetextfilter
plugins_DATA = ui.rc

kde_services_DATA = katetextfilter.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/katetextfilter.pot
