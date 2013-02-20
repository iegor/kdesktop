INCLUDES = $(all_includes)
METASOURCES = AUTO
KDE_ICON = AUTO

kde_module_LTLIBRARIES = libminitoolsplugin.la
libminitoolsplugin_la_SOURCES = minitoolsplugin.cpp
libminitoolsplugin_la_LIBADD = $(LIB_KHTML) -lkonq
libminitoolsplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

pluginsdir = $(kde_datadir)/khtml/kpartplugins
plugins_DATA = minitoolsplugin.rc minitoolsplugin.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/minitoolsplugin.pot
