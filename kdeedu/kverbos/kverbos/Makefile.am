SUBDIRS = icons data

bin_PROGRAMS = kverbos
kverbos_SOURCES = kfeedercontrol.cpp kresult.cpp qresult.ui verbspanish.cpp kverbosuser.cpp kerfassen.cpp qerfassen.ui kverbedit.cpp qverbedit.ui qverbosoptions.ui kverbosoptions.cpp qlernen.ui kstartuplogo.cpp kverbosview.cpp kverbosdoc.cpp kverbos.cpp main.cpp prefs.kcfgc
kverbos_LDADD   =  $(LIB_KFILE) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET)


EXTRA_DIST = kfeedercontrol.cpp kfeedercontrol.h main.cpp kverbos.cpp kverbos.h kverbosdoc.cpp kverbosdoc.h kverbosview.cpp kverbosview.h kverbosui.rc kverbos.desktop kstartuplogo.cpp kstartuplogo.h startuplogo.png qlernen.ui kverbosoptions.cpp kverbosoptions.h qverbosoptions.ui qverbedit.ui kverbedit.cpp kverbedit.h qerfassen.ui kerfassen.cpp kerfassen.h deu16.png span16.png kverbosuser.cpp kverbosuser.h verbspanish.cpp verbspanish.h qresult.ui kverbosuser.png kresult.cpp kresult.h

xdg_apps_DATA = kverbos.desktop
kde_kcfg_DATA = kverbos.kcfg

pics_DATA = startuplogo.png
picsdir = $(kde_datadir)/kverbos/pics

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO

# the library search path. 
kverbos_LDFLAGS = $(all_libraries) $(KDE_RPATH)

rcdir = $(kde_datadir)/kverbos
rc_DATA = kverbosui.rc

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kverbos.pot; \
	fi

