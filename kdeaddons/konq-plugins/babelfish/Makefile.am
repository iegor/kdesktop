INCLUDES    = $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = libbabelfishplugin.la

libbabelfishplugin_la_SOURCES = plugin_babelfish.cpp
libbabelfishplugin_la_LIBADD  = $(LIB_KPARTS) $(LIB_KHTML)
libbabelfishplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries) -no-undefined

pluginsdir   = $(kde_datadir)/khtml/kpartplugins
plugins_DATA = plugin_babelfish.rc plugin_babelfish.desktop

appsdir = $(kde_appsdir)/.hidden
apps_DATA = plugin_babelfish.desktop 

install-data-local: $(srcdir)/../uninstall.desktop
	$(mkinstalldirs) $(DESTDIR)$(pluginsdir)
	$(INSTALL_DATA) $(srcdir)/../uninstall.desktop $(DESTDIR)$(pluginsdir)/babelfishplugin.desktop

KDE_ICON = babelfish

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/babelfish.pot

kde_conf_DATA = translaterc
