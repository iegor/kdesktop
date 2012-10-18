INCLUDES = -I$(top_srcdir)/noatun/library/noatun \
	-I$(top_srcdir)/noatun/library \
	-I$(top_srcdir)/arts/gui/kde -I$(top_builddir)/arts/gui/kde \
	-I$(top_srcdir)/arts/gui/common -I$(top_builddir)/arts/gui/common \
	-I$(kde_includes)/arts \
	-I$(top_srcdir)/noatun/library/noatunarts \
	-I$(top_builddir)/noatun/library/noatunarts \
	 $(all_includes)

SUBDIRS= noatunarts . noatuntags noatun

lib_LTLIBRARIES = libnoatun.la libnoatuncontrols.la

libnoatun_la_SOURCES = pluginmodule.cpp cmodule.cpp downloader.cpp engine.cpp \
	playlist.cpp pref.cpp \
	player.cpp playlistsaver.cpp app.cpp \
	pluginloader.cpp plugin.cpp \
	noatunstdaction.cpp conversion.cpp\
	noatunui.cpp effectview.cpp \
	equalizerwidget.ui equalizerview.cpp equalizer.cpp \
	effects.cpp mimetypetree.cpp stereobuttonaction.cpp ksaver.cpp \
	video.cpp vequalizer.cpp spline.cpp titleproxy.cpp

include_HEADERS  = \
	cmodule.h plugin_deps.h equalizerview.h effectview.h mimetypetree.h \
	ksaver.h

libnoatun_la_LDFLAGS     = \
		-version-info 3:0:2 $(KDE_RPATH) $(all_libraries)
libnoatun_la_LIBADD      = $(top_builddir)/arts/gui/common/libartsgui.la \
	$(top_builddir)/arts/gui/kde/libartsgui_kde.la $(top_builddir)/arts/modules/libartsmodules.la \
	-lkio -lqtmcop -lkmedia2_idl $(top_builddir)/noatun/library/noatunarts/libnoatunarts.la \
	-lartsflow -lsoundserver_idl -lartskde $(LIBDL)

libnoatuncontrols_la_SOURCES = controls.cpp scrollinglabel.cpp
libnoatuncontrols_la_LDFLAGS = -version-info 3:0:2 $(KDE_RPATH) $(all_libraries) -no-undefined
libnoatuncontrols_la_LIBADD  = $(LIBDL) $(LIB_KDEUI)

METASOURCES = AUTO
META_INCLUDES = $(srcdir)/noatun

noinst_PROGRAMS = gentable
gentable_SOURCES = gentable.cpp
gentable_LDFLAGS = $(KDE_EXTRA_RPATH)

magictable: gentable
	$(top_builddir)/noatun/library/gentable > magictable

magictabledir = $(kde_datadir)/noatun
magictable_DATA = magictable

effects.lo: noatunarts/noatunarts.h ../../arts/gui/common/artsgui.h
engine.lo: noatunarts/noatunarts.h
equalizer.lo: noatunarts/noatunarts.h
plugin.lo: noatunarts/noatunarts.h
vequalizer.lo: noatunarts/noatunarts.h

