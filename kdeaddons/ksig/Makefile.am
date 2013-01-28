bin_PROGRAMS = ksig

ksig_SOURCES = siggenerator.cpp siglistview.cpp siglistviewitem.cpp ksig.cpp \
	standardtextbase.ui standardtext.cpp main.cpp 

INCLUDES = $(all_includes)

METASOURCES = AUTO
KDE_ICON = AUTO

ksig_LDFLAGS = $(all_libraries) $(KDE_RPATH)
ksig_LDADD   =  $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI)

rcdir = $(kde_datadir)/ksig
rc_DATA = ksigui.rc

xdg_apps_DATA = ksig.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/ksig.pot
