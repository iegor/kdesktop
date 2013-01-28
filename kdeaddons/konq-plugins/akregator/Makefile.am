INCLUDES    = $(all_includes)
METASOURCES = AUTO


# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = libakregatorkonqplugin.la libakregatorkonqfeedicon.la

libakregatorkonqplugin_la_SOURCES = akregatorplugin.cpp pluginbase.cpp
libakregatorkonqplugin_la_LIBADD = -lkonq $(LIB_KHTML)
libakregatorkonqplugin_la_LDFLAGS = -module $(KDE_PLUGIN) -avoid-version -no-undefined $(all_libraries)

libakregatorkonqfeedicon_la_SOURCES = konqfeedicon.cpp feeddetector.cpp pluginbase.cpp
libakregatorkonqfeedicon_la_LIBADD = -lkonq $(LIB_KHTML)
libakregatorkonqfeedicon_la_LDFLAGS = -module $(KDE_PLUGIN) -avoid-version -no-undefined $(all_libraries)

konqplugindir   = $(kde_servicesdir)
konqplugin_DATA = akregator_konqplugin.desktop
konqfeedicondir = $(kde_datadir)/khtml/kpartplugins
konqfeedicon_DATA = akregator_konqfeedicon.desktop akregator_konqfeedicon.rc

rssicondir = $(kde_datadir)/akregator/pics
rssicon_DATA = rss.png

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/akregator_konqplugin.pot
