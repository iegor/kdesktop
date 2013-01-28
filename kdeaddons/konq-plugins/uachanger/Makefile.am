kde_module_LTLIBRARIES = libuachangerplugin.la

AM_CPPFLAGS = $(all_includes)
METASOURCES = AUTO

libuachangerplugin_la_SOURCES = uachangerplugin.cpp
libuachangerplugin_la_LIBADD = $(LIB_KPARTS) $(LIB_KHTML)
libuachangerplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

pluginsdir = $(kde_datadir)/khtml/kpartplugins
plugins_DATA = uachangerplugin.rc uachangerplugin.desktop

appsdir = $(kde_appsdir)/.hidden
apps_DATA = uachangerplugin.desktop 

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/uachangerplugin.pot
