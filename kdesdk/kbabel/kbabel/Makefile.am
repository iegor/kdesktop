# this has all of the subdirectories that make will recurse into.  if
# there are none, comment this out
SUBDIRS = pics icons

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
noinst_LTLIBRARIES = libkbabel.la
bin_PROGRAMS = kbabel

# set the include path for X, qt and KDE
INCLUDES         = -I$(srcdir)/../common -I../common -I$(srcdir)/../commonui -I../commonui -I$(srcdir)/../kbabeldict -I../kbabeldict $(all_includes)


# which sources should be compiled for kbabel
libkbabel_la_SOURCES =	kbbookmarkhandler.cpp \
			kbcatalog.cpp headerwidget.ui headereditor.cpp  \
			spelldlgwidget.ui spelldlg.cpp gotodialog.cpp\
			kbhighlighting.cpp mymultilineedit.cpp \
			hidingmsgedit.cpp 	\
			kbabelpref.cpp 		\
			kbcatalogview.cpp	\
			commentview.cpp 	\
			contextview.cpp		\
			charselectview.cpp	\
			taglistview.cpp		\
			sourceview.cpp		\
			kbabelview.cpp kbabelview2.cpp \
			kbabel.cpp kbabeliface.skel kbabelsplash.cpp \
			kbabelsettings.kcfgc			\
			fontpreferences.ui			\
			editordiffpreferences.ui		\
			editorpreferences.ui			\
			searchpreferences.ui			\
			colorpreferences.ui			\
			kbcataloglistview.cpp kbcataloglistviewitem.cpp \
			errorlistview.cpp	

kde_kcfg_DATA=kbabel.kcfg

libkbabel_la_LIBADD =  ../commonui/libkbabelcommonui.la ../kbabeldict/libkbabeldict.la $(LIB_KIO) $(LIB_KSPELL)
libkbabel_la_LDFLAGS = $(all_libraries)


kbabel_SOURCES = main.cpp

# the libraries to link against.
kbabel_LDADD   = libkbabel.la
kbabel_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# these are the headers for your project
noinst_HEADERS   = kbabel.h kbabelview.h \
		   kbcatalogview.h	\
		   commentview.h	\
		   contextview.h	\
		   charselectview.h	\
		   taglistview.h	\
		   sourceview.h		\
		   kbabeliface.h kbabelpref.h\
                   headereditor.h mymultilineedit.h\
                   gotodialog.h hidingmsgedit.h\
                   kbcatalog.h spelldlg.h\
                   kbabelsplash.h kbbookmarkhandler.h \
		   kbhighlighting.h	\
		   kbcataloglistview.h	\
		   kbcataloglistviewitem.h	\
		   errorlistview.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

rcdir = $(kde_datadir)/kbabel
rc_DATA = kbabelui.rc

update_DATA = kbabel-project.upd kbabel-difftoproject.upd
updatedir = $(kde_datadir)/kconf_update

api:
	mkdir -p API && kdoc -d API -u $$PWD/API -p  -lkdeui -lkdecore -lqt -ldcop $(noinst_HEADERS)

distclean-local:
	rm -r -f API

KDE_ICON = kbabel

 # this is where the kdelnk file will go
xdg_apps_DATA = kbabel.desktop

kbabel.lo: ../common/kbprojectsettings.h ../common/version.h
kbabelview.lo: ../common/kbprojectsettings.h ../common/version.h
kbabelview2.lo: ../common/kbprojectsettings.h
main.o: ../common/version.h

