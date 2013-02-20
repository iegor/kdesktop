INCLUDES = $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = libcrashesplugin.la
libcrashesplugin_la_SOURCES = crashesplugin.cpp
libcrashesplugin_la_LIBADD = $(LIB_KHTML) -lkonq
libcrashesplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

pluginsdir = $(kde_datadir)/khtml/kpartplugins
plugins_DATA = crashesplugin.rc crashesplugin.desktop

appsdir = $(kde_appsdir)/.hidden
apps_DATA = crashesplugin.desktop 

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/crashesplugin.pot
