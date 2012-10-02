## Makefile.am for pocompendium

# this has all of the subdirectories that make will recurse into.  if
# there are none, comment this out
#SUBDIRS = 


# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
kde_module_LTLIBRARIES = kbabeldict_pocompendium.la

# set the include path for X, qt and KDE
INCLUDES         = -I$(srcdir)/../.. -I../../../common -I$(srcdir)/../../../common $(all_includes)


# which sources should be compiled for kbabel
kbabeldict_pocompendium_la_SOURCES = pocompendium.cpp preferenceswidget.cpp \
                             pc_factory.cpp pwidget.ui compendiumdata.cpp

kbabeldict_pocompendium_la_LIBADD =  ../../libkbabeldictplugin.la ../../../common/libkbabelcommon.la $(LIB_KDEUI) $(LIB_KIO)
kbabeldict_pocompendium_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined



# these are the headers for your project
noinst_HEADERS   = pocompendium.h preferenceswidget.h pc_factory.h \
                   compendiumdata.h


# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO


# this is where the kdelnk file will go
#datadir   = $(kde_datadir)/kbabeldict/modules
#data_DATA = pocompendium.rc

kde_services_DATA = pocompendium.desktop
EXTRA_DIST = $(kde_services_DATA)
