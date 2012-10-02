## Makefile.am for DBSE2

# this has all of the subdirectories that make will recurse into.  if
# there are none, comment this out
#SUBDIRS = 


# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
kde_module_LTLIBRARIES = kbabeldict_dbsearchengine2.la

# set the include path for X, qt and KDE
INCLUDES         = -I$(srcdir)/../.. -I$(srcdir)/../../../common $(all_includes) -I$(DBIV_INCLUDES) 

# which sources should be compiled for kbabel
kbabeldict_dbsearchengine2_la_SOURCES = dbscan.cpp preferenceswidget.cpp dbse2.ui KDBSearchEngine2.cpp database.cpp dbentries.cpp dbse2_factory.cpp sourcedialog.ui algorithms.cpp chunk.cpp

#kbabeldict_dbsearchengine2_la_SOURCES = KDBSearchEngine2.cpp  database.cpp dbentries.cpp dbse2_factory.cpp  
kbabeldict_dbsearchengine2_la_LIBADD =  ../../libkbabeldictplugin.la ../../../common/libkbabelcommon.la $(LIB_KDEUI) $(LIB_KIO) $(LIB_DBIV)_cxx
kbabeldict_dbsearchengine2_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined -L$(DBIV_LDFLAGS)


# these are the headers for your project
noinst_HEADERS = KDBSearchEngine2.h database.h dbentries.h dbse2_factory.h 
#chunk.h algorithms.h


# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

kde_services_DATA = dbsearchengine2.desktop
EXTRA_DIST = $(kde_services_DATA)


