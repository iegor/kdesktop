## Makefile.am for DBSE

# this has all of the subdirectories that make will recurse into.  if
# there are none, comment this out
#SUBDIRS = 


# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
kde_module_LTLIBRARIES = kbabeldict_dbsearchengine.la

# set the include path for X, qt and KDE
INCLUDES         = -I$(srcdir)/../.. -I../../../common -I$(srcdir)/../../../common $(all_includes) $(DBIV_INCLUDES) 

# which sources should be compiled for kbabel
kbabeldict_dbsearchengine_la_SOURCES = KDBSearchEngine.cpp preferenceswidget.cpp \
	dbse_factory.cpp dbseprefwidget.ui database.cpp dbscan.cpp
#database.cpp dbscan.cpp
kbabeldict_dbsearchengine_la_LIBADD =  ../../libkbabeldictplugin.la ../../../common/libkbabelcommon.la $(LIB_KDEUI) $(LIB_KIO) $(LIB_DBIV)
kbabeldict_dbsearchengine_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined \
	$(DBIV_LDFLAGS)


# these are the headers for your project
noinst_HEADERS   = KDBSearchEngine.h preferenceswidget.h dbse_factory.h \
	 database.h dbscan.h


# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

kde_services_DATA = dbsearchengine.desktop
EXTRA_DIST = $(kde_services_DATA)

