## Makefile.am for KBabel catalogmanager

# this has all of the subdirectories that make will recurse into. If
# there are none, comment this out
SUBDIRS = libcvs libsvn icons

# this is the program that gets installed. Its name is used for all
# of the other Makefile.am variables
noinst_LTLIBRARIES = libcatalogmanager.la
bin_PROGRAMS = catalogmanager

# set the include path for X, qt and KDE. Let $(all_includes) be always last.
INCLUDES         = -I$(srcdir)/../common -I../common -I$(srcdir)/../kbabeldict \
-I$(srcdir)/../commonui -I../commonui -I./libsvn -I./libcvs \
-I$(srcdir)/libsvn -I$(srcdir)/libcvs $(all_includes)


# which sources should be compiled for kbabel
libcatalogmanager_la_SOURCES =	catalogmanageriface.skel \
			validationoptions.ui \
			catalogmanagerview.cpp \
			catalogmanager.cpp findinfilesdialog.cpp \
			catmanlistitem.cpp multiroughtransdlg.cpp validateprogresswidget.ui \
			validateprogress.cpp markpatternwidget.ui markpatterndialog.cpp


libcatalogmanager_la_LIBADD =  ../commonui/libkbabelcommonui.la \
../kbabeldict/libkbabeldict.la ./libcvs/libcatalogmanagercvs.la \
./libsvn/libcatalogmanagersvn.la $(LIB_KIO)
libcatalogmanager_la_LDFLAGS = $(all_libraries) -no-undefined


catalogmanager_SOURCES = main.cpp

# the libraries to link against.
catalogmanager_LDADD   = libcatalogmanager.la
catalogmanager_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# these are the headers for your project
noinst_HEADERS   = catalogmanageriface.h catalogmanager.h \
		   catalogmanagerview.h catalogmanagerapp.h findinfilesdialog.h \
		   catmanlistitem.h catmanresource.h multiroughtransdlg.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

rcdir = $(kde_datadir)/catalogmanager
rc_DATA = catalogmanagerui.rc


api:
	mkdir -p API && kdoc -d API -u $$PWD/API -p  -lkdeui -lkdecore -lqt -ldcop $(noinst_HEADERS)

distclean-local:
	rm -r -f API

KDE_ICON = AUTO

# this is where the kdelnk file will go
xdg_apps_DATA = catalogmanager.desktop

catalogmanager.lo: ../common/version.h
main.o: ../common/version.h

