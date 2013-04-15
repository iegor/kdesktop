# Here resides the class view part.

INCLUDES = -I$(top_srcdir)/lib/compat -I$(top_srcdir)/lib/interfaces -I$(top_srcdir)/lib/interfaces/extensions \
	-I$(top_srcdir)/lib/interfaces/external -I$(top_srcdir)/lib/util -I$(top_srcdir)/lib/widgets $(all_includes)

kde_module_LTLIBRARIES = libvsslnexplorer.la
libvsslnexplorer_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libvsslnexplorer_la_LIBADD = $(top_builddir)/lib/libkdevelop.la $(top_builddir)/lib/widgets/libkdevwidgets.la $(LIB_KHTML) $(top_builddir)/lib/interfaces/extensions/libkdevextensions.la

libvsslnexplorer_la_SOURCES = vstudiopart.cpp vsslnexplorerwidget.cpp \
	digraphview.cpp hierarchydlg.cpp navigator.cpp viewcombos.cpp

METASOURCES = AUTO

servicedir = $(kde_servicesdir)
service_DATA = vsslnexplorer.kdesktop

rcdir = $(kde_datadir)/kdevvstudio
rc_DATA = kdevvstudio.rc

QTDIR = /home/bernd/kdesrc/qt-copy

foo: digraphview.cpp digraphview.h
	$(QTDIR)/bin/moc digraphview.h > digraphview.moc ; \
	g++ digraphview.cpp -I$(QTDIR)/include -L$(QTDIR)/lib -L/usr/X11R6/lib -lqt -lX11 -lXext -o foo
noinst_HEADERS = viewcombos.h navigator.h
