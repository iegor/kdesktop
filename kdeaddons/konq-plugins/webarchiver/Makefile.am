INCLUDES    = $(all_includes)
KDE_CXXFLAGS = $(USE_EXCEPTIONS)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = libwebarchiverplugin.la webarchivethumbnail.la

libwebarchiverplugin_la_SOURCES = plugin_webarchiver.cpp archiveviewbase.ui archivedialog.cpp
libwebarchiverplugin_la_LIBADD  = $(LIB_KPARTS) $(LIB_KHTML)
libwebarchiverplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

pluginsdir = $(kde_datadir)/khtml/kpartplugins
plugins_DATA = plugin_webarchiver.rc plugin_webarchiver.desktop

appsdir = $(kde_appsdir)/.hidden
apps_DATA = plugin_webarchiver.desktop 

install-data-local: $(srcdir)/../uninstall.desktop
	$(mkinstalldirs) $(DESTDIR)$(pluginsdir)
	$(INSTALL_DATA) $(srcdir)/../uninstall.desktop $(DESTDIR)$(pluginsdir)/webarchiverplugin.desktop

webarchivethumbnail_la_SOURCES = webarchivecreator.cpp
webarchivethumbnail_la_LIBADD = $(LIB_KHTML)
webarchivethumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

servicesdir = $(kde_servicesdir)
services_DATA = webarchivethumbnail.desktop

KDE_ICON = webarchiver

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/webarchiver.pot
