INCLUDES    = $(all_includes)
METASOURCES = AUTO


# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = libmfkonqmficon.la

libmfkonqmficon_la_SOURCES = konqmficon.cpp pluginbase.cpp
libmfkonqmficon_la_LIBADD = -lkonq $(LIB_KHTML)
libmfkonqmficon_la_LDFLAGS = -module $(KDE_PLUGIN) -avoid-version -no-undefined $(all_libraries)

konqmficondir = $(kde_datadir)/khtml/kpartplugins
konqmficon_DATA = mf_konqmficon.desktop mf_konqmficon.rc

mficondir = $(kde_datadir)/microformat/pics
mficon_DATA = microformat.png

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/mf_konqplugin.pot
