INCLUDES    =  $(all_includes)
METASOURCES = AUTO

#KDE_ICON = textfilter

# Install this plugin in the KDE modules directory
#kde_module_LTLIBRARIES = katefll_initplugin.la katefll_plugin.la
kde_module_LTLIBRARIES = katefll_plugin.la

# This is all standard.  Remove the LIB_KHTML reference if you are not
# using the KHTML Part
#katefll_initplugin_la_SOURCES = katefll_initplugin.cpp
#katefll_initplugin_la_LIBADD = -lkateinterfaces
#katefll_initplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

katefll_plugin_la_SOURCES = katefll_plugin.cpp
katefll_plugin_la_LIBADD = -lkateinterfaces
katefll_plugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# Install the .rc file in the Part's directory (in this case, the part
# is KHTMLPart)
pluginsdir = $(kde_datadir)/kate/plugins/katefll
plugins_DATA = ui.rc

#kde_services_DATA = katefll_initplugin.desktop katefll_plugin.desktop
kde_services_DATA = katefll_plugin.desktop

kdelnk_DATA = katefll.desktop
kdelnkdir = $(kde_appsdir)/.hidden

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/katefll_initplugin.pot
	$(XGETTEXT) *.cpp -o $(podir)/katefll_plugin.pot

