INCLUDES= -I$(top_srcdir)/kbugbuster/backend $(all_includes)

if include_kcalresource
KRESOURCES_SUBDIR = kresources
endif

SUBDIRS = backend gui pics $(KRESOURCES_SUBDIR)

bin_PROGRAMS = kbugbuster

kbugbuster_SOURCES = main.cpp
kbugbuster_LDADD   = -lkutils gui/libkbbmainwindow.la \
                     backend/libkbbbackend.la $(LIB_KHTML) $(LIB_KIO) 
kbugbuster_LDFLAGS = $(all_libraries) $(KDE_RPATH)

xdg_apps_DATA = kbugbuster.desktop

KDE_ICON = kbugbuster

METASOURCES = AUTO

EXTRA_DIST = AUTHORS COPYING ChangeLog INSTALL README TODO kbugbuster.lsm 

messages: rc.cpp
	$(EXTRACTRC) */*.kcfg >> rc.cpp
	$(EXTRACTRC) gui/*.rc >> rc.cpp
	$(EXTRACTRC) gui/*.ui >> rc.cpp
	$(XGETTEXT) *.cpp */*.h */*.cpp -o $(podir)/kbugbuster.pot
