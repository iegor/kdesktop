SUBDIRS = icons

INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = libsearchbarplugin.la

libsearchbarplugin_la_SOURCES = searchbar.cpp
libsearchbarplugin_la_LIBADD = $(LIB_KHTML)
libsearchbarplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

pluginsdir = $(kde_datadir)/konqueror/kpartplugins
plugins_DATA = searchbar.rc searchbar.desktop

install-data-local: $(srcdir)/../uninstall.desktop
	$(mkinstalldirs) $(DESTDIR)$(pluginsdir)
	$(INSTALL_DATA) $(srcdir)/../uninstall.desktop $(DESTDIR)$(pluginsdir)/searchbarplugin.desktop

METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/searchbarplugin.pot
