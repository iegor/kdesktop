INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KSYCOCA) $(LIB_KFILE)

SUBDIRS = designer pics

EXTRA_DIST = atlantikdesigner.desktop

xdg_apps_DATA = atlantikdesigner.desktop

METASOURCES = AUTO 

rcdir = $(kde_datadir)/atlantikdesigner
rc_DATA = atlantikdesignerui.rc

messages: rc.cpp
	$(XGETTEXT) `find . -name '*.cpp'` -o $(podir)/atlantikdesigner.pot
