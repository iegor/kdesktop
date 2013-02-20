INCLUDES= $(all_includes)
METASOURCES = AUTO

KDE_ICON = fsview

EXTRA_DIST = main.cpp fsview.cpp fsview.h scan.cpp scan.h scantest.cpp \
	inode.h inode.cpp \
	fsview.desktop hi32-app-fsview.png

#xdg_apps_DATA = fsview.desktop

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.cpp`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/fsview.pot; \
	fi

# Used both by application and KPart

noinst_LTLIBRARIES = libfsview.la
libfsview_la_SOURCES = treemap.cpp fsview.cpp scan.cpp inode.cpp

# Application

bin_PROGRAMS = fsview

fsview_LDFLAGS = $(all_libraries) $(KDE_RPATH)
fsview_SOURCES = main.cpp
fsview_LDADD   = libfsview.la $(LIB_KIO) $(LIB_KDECORE) $(LIB_QT)

check_PROGRAMS = scantest

scantest_LDFLAGS = $(all_libraries) $(KDE_RPATH)
scantest_SOURCES = scantest.cpp
scantest_LDADD   = libfsview.la $(LIB_KIO) $(LIB_KDECORE) $(LIB_QT)

# The KPart

kde_module_LTLIBRARIES = libfsviewpart.la
libfsviewpart_la_SOURCES = fsview_part.cpp
libfsviewpart_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
libfsviewpart_la_LIBADD  = libfsview.la $(LIB_KPARTS) $(LIB_KIO) -lkonq

partdesktopdir   = $(kde_servicesdir)
partdesktop_DATA = fsview_part.desktop

rcdir = $(kde_datadir)/fsview
rc_DATA = fsview_part.rc

appsdir = $(kde_appsdir)/.hidden
apps_DATA = fsview.desktop
