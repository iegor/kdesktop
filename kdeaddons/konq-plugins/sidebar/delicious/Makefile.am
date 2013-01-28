SUBDIRS = . pics

METASOURCES = AUTO
INCLUDES = -I$(kde_includes)/kio $(all_includes)

################################
# these are the headers for your project
noinst_HEADERS = plugin.h mainWidget.h tagListItem.h bookmarkListItem.h

kde_module_LTLIBRARIES = konqsidebar_delicious.la

# the Part's source, library search path, and link libraries
konqsidebar_delicious_la_SOURCES = plugin.cpp widget.ui mainWidget.cpp plugin.skel tagListItem.cpp \
	bookmarkListItem.cpp
konqsidebar_delicious_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
konqsidebar_delicious_la_LIBADD  = $(LIB_KPARTS) $(LIB_KIO) -lkonqsidebarplugin

globaladddir   = $(kde_datadir)/konqsidebartng/add
globaladd_DATA = delicious_add.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/konqsidebar_delicious.pot
