bin_PROGRAMS = kpartsaver.kss
INCLUDES = $(all_includes)

kpartsaver_kss_SOURCES = kpartsaver.cpp configwidget.ui
kpartsaver_kss_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kpartsaver_kss_LDADD = $(LIB_KFILE) $(LIB_KPARTS) -lkscreensaver

METASOURCES = AUTO

desktop_DATA = kpartsaver.desktop
desktopdir = $(kde_appsdir)/System/ScreenSavers

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kpartsaver.pot
