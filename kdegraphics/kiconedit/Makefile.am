INCLUDES = $(all_includes)
SUBDIRS = pics

bin_PROGRAMS = kiconedit

kiconedit_SOURCES = utils.cpp main.cpp kiconedit.cpp kicongrid.cpp \
    kiconcolors.cpp kcolorgrid.cpp palettetoolbar.cpp\
    kicon.cpp kresize.cpp knew.cpp properties.cpp \
    kiconeditslots.cpp kiconconfig.cpp 

kiconedit_METASOURCES = AUTO

# the library search path. 
kiconedit_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kiconedit_LDADD = $(LIB_KFILE) -lkdeprint

DISTCLEANFILES = *~ .deps/* .libs/*

rcdir = $(kde_datadir)/kiconedit
rc_DATA = kiconeditui.rc

xdg_apps_DATA = kiconedit.desktop

messages: rc.cpp
	$(XGETTEXT) rc.cpp $(kiconedit_SOURCES) -o $(podir)/kiconedit.pot

