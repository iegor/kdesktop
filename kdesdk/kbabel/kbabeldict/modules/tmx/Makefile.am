## Makefile.am for tmxcompendium

# this has all of the subdirectories that make will recurse into.  if
# there are none, comment this out
#SUBDIRS = 


# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
kde_module_LTLIBRARIES = kbabeldict_tmxcompendium.la

# set the include path for X, qt and KDE
INCLUDES         = -I$(srcdir)/../.. -I../../../common -I$(srcdir)/../../../common $(all_includes)


# which sources should be compiled for kbabel
kbabeldict_tmxcompendium_la_SOURCES = tmxcompendium.cpp preferenceswidget.cpp \
                             pc_factory.cpp pwidget.ui tmxcompendiumdata.cpp

kbabeldict_tmxcompendium_la_LIBADD =  ../../libkbabeldictplugin.la ../../../common/libkbabelcommon.la $(LIB_KDEUI) $(LIB_KIO)
kbabeldict_tmxcompendium_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined



# these are the headers for your project
noinst_HEADERS   = tmxcompendium.h preferenceswidget.h pc_factory.h \
                   tmxcompendiumdata.h


# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

kde_services_DATA = tmxcompendium.desktop
EXTRA_DIST = $(kde_services_DATA)
