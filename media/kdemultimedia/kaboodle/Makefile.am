INCLUDES = -I$(kde_includes)/kio -I$(kde_includes)/arts $(all_includes)

SUBDIRS = pics actions

kde_module_LTLIBRARIES = libkaboodlepart.la

noinst_LTLIBRARIES = libkaboodle_noinst.la
libkaboodle_noinst_la_SOURCES = conf.cpp \
	controls.cpp \
	engine.cpp \
	player.cpp \
	view.cpp \
	kaboodle_factory.cpp

libs = $(LIB_KPARTS) -lqtmcop -lkmedia2_idl -lsoundserver_idl -lartskde -lkmediaplayer

libkaboodlepart_la_SOURCES = dummy.cpp
libkaboodlepart_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
libkaboodlepart_la_LIBADD = libkaboodle_noinst.la $(libs)

kaboodle_SOURCES = main.cpp \
	kaboodleapp.cpp \
	userinterface.cpp 

dummy.cpp:
	echo > dummy.cpp

CLEANFILES = dummy.cpp

kaboodle_LDADD = libkaboodle_noinst.la $(libs)
kaboodle_LDFLAGS = $(all_libraries) $(KDE_RPATH)

bin_PROGRAMS = kaboodle

METASOURCES = AUTO

xdg_apps_DATA = kaboodle.desktop

kde_services_DATA = kaboodle_component.desktop kaboodleengine.desktop

appdata_DATA = kaboodleui.rc kaboodlepartui.rc
appdatadir = $(kde_datadir)/kaboodle

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kaboodle.pot
