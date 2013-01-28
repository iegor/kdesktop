# set the include path for X, qt and KDE
INCLUDES= $(all_includes)
METASOURCES = AUTO 

kde_module_LTLIBRARIES = katepybrowseplugin.la

katepybrowseplugin_la_SOURCES = kpybrowser.cpp pybrowsenode.cpp pybrowse_part.cpp
katepybrowseplugin_la_LIBADD = -lkateinterfaces
katepybrowseplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

pluginsdir = $(kde_datadir)/kate/plugins/katepybrowse
plugins_DATA = ui.rc

kde_services_DATA = katepybrowse.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/katepybrowse.pot

