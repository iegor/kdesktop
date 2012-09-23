SUBDIRS = data

INCLUDES= $(all_includes)

####### Files

bin_PROGRAMS = 	kghostview
lib_LTLIBRARIES = libkghostviewlib.la
kde_module_LTLIBRARIES = libkghostviewpart.la
noinst_LTLIBRARIES = libdscparse.la

libkghostviewlib_la_LDFLAGS = $(all_libraries)
libkghostviewlib_la_LIBADD = $(LIB_KFILE) $(LIB_KPARTS) -lkdeprint libdscparse.la

libkghostviewpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
libkghostviewpart_la_LIBADD = libkghostviewlib.la

# Check "make final" after making changes to the following line!!
libkghostviewlib_la_SOURCES = kgvshell.cpp kgvdocument.cpp kgv_miniwidget.cpp \
	marklist.cpp logwindow.cpp infodialog.cpp \
	kgvpageview.cpp ps.c kgv_view.cpp scrollbox.cpp  kgvpagedecorator.cpp \
	kgvconfigdialog.cpp kgvmainwidget.cpp \
	kdscerrordialog.cpp displayoptions.cpp kpswidget.cpp \
	fullscreenfilter.cpp kgvfactory.cpp \
	generalsettingswidget.ui gssettingswidget.ui thumbnailservice.cpp \
	configuration.kcfgc

libkghostviewpart_la_SOURCES = part_init.cpp

kghostview_SOURCES = main.cpp
kghostview_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kghostview_LDADD = libkghostviewlib.la

libdscparse_la_LDFLAGS = $(all_libraries) -no-undefined
libdscparse_la_LIBADD = $(LIB_QT)
libdscparse_la_SOURCES = dscparse.cpp dscparse_adapter.cpp

noinst_HEADERS =  marklist.h logwindow.h infodialog.h kgvshell.h \
	kpswidget.h kgvpageview.h ps.h kgv_miniwidget.h kgv_view.h scrollbox.h \
	kgvpagedecorator.h kgvconfigdialog.h kgvmainwidget.h dscparse.h \
	dscparse_adapter.h kdscerrordialog.h kgvdocument.h displayoptions.h \
	fullscreenfilter.h kgvfactory.h thumbnailservice.h

METASOURCES = AUTO
EXTRA_DIST = kghostview.desktop 

KDE_ICON = kghostview

xdg_apps_DATA = kghostview.desktop
kde_kcfg_DATA = kghostview.kcfg

partdir = $(kde_datadir)/kghostview
part_DATA = kgv_part.rc kghostviewui.rc

partdesktopdir   = $(kde_servicesdir)
partdesktop_DATA = kghostview_part.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kghostview.pot


updatedir = $(kde_datadir)/kconf_update
update_DATA = kghostview.upd
update_SCRIPTS = update-to-xt-names.pl

# check_PROGRAMS = testdsc

# testdsc_SOURCES = testdsc.cpp kdsc.cpp ps.c
# testdsc_LDADD = -lqt
