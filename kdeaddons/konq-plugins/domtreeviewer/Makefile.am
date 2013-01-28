INCLUDES    = $(all_includes)

# Needed to catch DOM exceptions
KDE_CXXFLAGS = $(USE_EXCEPTIONS)
# -DKListView=DOMListView

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = libdomtreeviewerplugin.la

libdomtreeviewerplugin_la_SOURCES = 	plugin_domtreeviewer.cpp \
					domtreeview.cpp \
					domlistviewitem.cpp \
					domtreewindow.cpp \
					domtreecommands.cpp \
					signalreceiver.cpp \
					domtreeviewbase.ui \
					attributeeditdialog.ui \
					elementeditdialog.ui \
					texteditdialog.ui \
					messagedialog.ui
#					klistview.cpp \
#
libdomtreeviewerplugin_la_LIBADD  = $(LIB_KPARTS) $(LIB_KHTML)
libdomtreeviewerplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

pluginsdir   = $(kde_datadir)/khtml/kpartplugins
plugins_DATA = plugin_domtreeviewer.rc plugin_domtreeviewer.desktop

appsdir = $(kde_appsdir)/.hidden
apps_DATA = plugin_domtreeviewer.desktop 

install-data-local: $(srcdir)/../uninstall.desktop
	$(mkinstalldirs) $(DESTDIR)$(pluginsdir)
	$(INSTALL_DATA) $(srcdir)/../uninstall.desktop $(DESTDIR)$(pluginsdir)/domtreeviewerplugin.desktop

METASOURCES = AUTO
KDE_ICON = domtreeviewer

pluginsdatadir = $(kde_datadir)/domtreeviewer
pluginsdata_DATA = domtreeviewerui.rc

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/domtreeviewer.pot

