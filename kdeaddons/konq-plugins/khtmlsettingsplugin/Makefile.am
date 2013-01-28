INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = libkhtmlsettingsplugin.la

libkhtmlsettingsplugin_la_SOURCES = settingsplugin.cpp
libkhtmlsettingsplugin_la_LIBADD = $(LIB_KHTML)
libkhtmlsettingsplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

pluginsdir = $(kde_datadir)/khtml/kpartplugins
plugins_DATA = khtmlsettingsplugin.rc khtmlsettingsplugin.desktop

appsdir = $(kde_appsdir)/.hidden
apps_DATA = khtmlsettingsplugin.desktop 

METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/khtmlsettingsplugin.pot
