## Makefile.am for kbabeldict

# this has all of the subdirectories that make will recurse into.  if
# there are none, comment this out
SUBDIRS = . modules

pkgincludedir = $(includedir)/kbabel

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
noinst_LTLIBRARIES = libkbabeldict.la
bin_PROGRAMS = kbabeldict

# set the include path for X, qt and KDE
INCLUDES         = -I$(srcdir)/../common -I$(top_builddir)/kbabel/common $(all_includes)


# library for babeldict plugins
lib_LTLIBRARIES = libkbabeldictplugin.la
libkbabeldictplugin_la_SOURCES = searchengine.cpp 
libkbabeldictplugin_la_LIBADD =  $(LIB_KDECORE)
libkbabeldictplugin_la_LDFLAGS = -version-info 3:0:2 $(all_libraries)

libkbabeldict_la_SOURCES = kbabeldictbox.cpp \
            kbabeldictiface.skel dictionarymenu.cpp dictchooser.cpp \
            aboutmoduledlg.cpp 
libkbabeldict_la_LIBADD =  ../common/libkbabelcommon.la libkbabeldictplugin.la $(LIB_KDEUI)
libkbabeldict_la_LDFLAGS = $(all_libraries) -no-undefined


kbabeldict_SOURCES = main.cpp kbabeldictview.cpp kbabeldict.cpp kbabelsplash.cpp

# the libraries to link against.
kbabeldict_LDADD   = libkbabeldict.la
kbabeldict_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# these are the headers for your project
noinst_HEADERS   =  kbabeldict.h kbabeldictview.h kbabelsplash.h aboutmoduledlg.h
pkginclude_HEADERS = searchengine.h kbabeldictbox.h kbabeldictiface.h \
dictionarymenu.h dictchooser.h 


# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

KDE_OPTIONS = nofinal
#rcdir = $(kde_datadir)/kbabel
#rc_DATA = kbabelui.rc

api:
	mkdir -p API && kdoc -d API -u $$PWD/API -p  -lkdeui -lkdecore -lqt -ldcop *.h

distclean-local:
	rm -r -f API

KDE_ICON = kbabeldict

 # this is where the kdelnk file will go
xdg_apps_DATA = kbabeldict.desktop

kde_servicetypes_DATA = kbabeldict_module.desktop
EXTRA_DIST = $(kde_servicetypes_DATA)

kbabeldictbox.lo: ../common/version.h
main.o: ../common/version.h

