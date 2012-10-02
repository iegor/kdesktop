## Makefile.am for libkbabelcommon

# this has all of the subdirectories that make will recurse into.  if
# there are none, comment this out
SUBDIRS = libgettext

pkgincludedir = $(includedir)/kbabel

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
lib_LTLIBRARIES = libkbabelcommon.la

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# which sources should be compiled 
libkbabelcommon_la_SOURCES = catalog.cpp catalogitem.cpp editcmd.cpp\
        msgfmt.cpp catalogsettings.cpp tagextractor.cpp diff.cpp\
	argextractor.cpp kbmailer.cpp poinfo.cpp\
	regexpextractor.cpp importplugin.cpp \
	exportplugin.cpp 	\
	kbproject.cpp		\
	kbprojectmanager.cpp	\
	kbprojectsettings.kcfgc	\
	projectsettings.cpp	\
	stringdistance.cpp

libkbabelcommon_la_LIBADD =  $(LIB_KIO) libgettext/libgettext.la 
libkbabelcommon_la_LDFLAGS = $(all_libraries) -version-info 5:0:2 -no-undefined

# these are the headers for your project
noinst_HEADERS   =  resources.h version.h catalogitem_private.h catalog_private.h kbmailer.h stringdistance.h
pkginclude_HEADERS = catalog.h catalogitem.h editcmd.h msgfmt.h catalogsettings.h findoptions.h catalogview.h tagextractor.h poinfo.h\
	regexpextractor.h itempart.h catalogfileplugin.h kbabeldatatool.h kbproject.h kbprojectmanager.h \
	projectsettings.h


# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

# KConfigXT project settings
kde_kcfg_DATA=kbprojectsettings.kcfg

# service type for the filters
kde_servicetypes_DATA = kbabelfilter.desktop
EXTRA_DIST = $(kde_servicetypes_DATA)

KDE_OPTIONS = nofinal

version.h: $(srcdir)/../VERSION
	echo "/* Generated file - DO NOT EDIT */" > version.h
	echo "#undef VERSION" >> version.h
	echo "#define VERSION \"`cat $(srcdir)/../VERSION`\"" >> version.h

catalog.lo: version.h

# updating of project settings for default project
update_DATA = kbabel-projectrename.upd
updatedir = $(kde_datadir)/kconf_update

api:
	mkdir -p API && kdoc -d API -u $$PWD/API -p  -lkdecore -lqt-mt -ldcop $(pkginclude_HEADERS)	

distclean-local:
	rm -r -f API
	rm -f version.h

