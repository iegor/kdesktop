INCLUDES    =  $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = katecppsymbolviewerplugin.la

katecppsymbolviewerplugin_la_SOURCES = cpp_parser.cpp tcl_parser.cpp plugin_katesymbolviewer.cpp
katecppsymbolviewerplugin_la_LIBADD = -lkateinterfaces
katecppsymbolviewerplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

pluginsdir = $(kde_datadir)/kate/plugins/katecppsymbolviewer
plugins_DATA = ui.rc

kde_services_DATA = katecppsymbolviewer.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/katecppsymbolviewer.pot
