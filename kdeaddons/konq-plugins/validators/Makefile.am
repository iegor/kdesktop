INCLUDES    = $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = libvalidatorsplugin.la

libvalidatorsplugin_la_SOURCES = plugin_validators.cpp validatorsdialog.cpp
libvalidatorsplugin_la_LIBADD  = $(LIB_KPARTS) $(LIB_KHTML)
libvalidatorsplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

pluginsdir   = $(kde_datadir)/khtml/kpartplugins
plugins_DATA = plugin_validators.rc plugin_validators.desktop

appsdir = $(kde_appsdir)/.hidden
apps_DATA = plugin_validators.desktop 

install-data-local: $(srcdir)/../uninstall.desktop
	$(mkinstalldirs) $(DESTDIR)$(pluginsdir)
	$(INSTALL_DATA) $(srcdir)/../uninstall.desktop $(DESTDIR)$(pluginsdir)/validatorsplugin.desktop

KDE_ICON = validators cssvalidator htmlvalidator

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/validatorsplugin.pot
