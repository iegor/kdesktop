INCLUDES    =  $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kateinsertcommandplugin.la

# This is all standard.  Remove the LIB_KHTML reference if you are not
# using the KHTML Part
kateinsertcommandplugin_la_SOURCES = plugin_kateinsertcommand.cpp
kateinsertcommandplugin_la_LIBADD = -lkateinterfaces
kateinsertcommandplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# Install the .rc file in the Part's directory (in this case, the part
# is KHTMLPart)
pluginsdir = $(kde_datadir)/kate/plugins/kateinsertcommand
plugins_DATA = ui.rc

kde_services_DATA = kateinsertcommand.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kateinsertcommand.pot
