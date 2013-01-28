kde_module_LTLIBRARIES = libdirfilterplugin.la

AM_CPPFLAGS = $(all_includes)
METASOURCES = AUTO

libdirfilterplugin_la_SOURCES = dirfilterplugin.cpp
libdirfilterplugin_la_LIBADD = -lkonq
libdirfilterplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

iconviewdir = $(kde_datadir)/konqiconview/kpartplugins
iconview_DATA = dirfilterplugin.rc dirfilterplugin.desktop
listviewdir = $(kde_datadir)/konqlistview/kpartplugins
listview_DATA = dirfilterplugin.rc dirfilterplugin.desktop

appsdir = $(kde_appsdir)/.hidden
apps_DATA = dirfilterplugin.desktop 

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/dirfilterplugin.pot
