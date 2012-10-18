SUBDIRS = mcopclass

lib_LTLIBRARIES = libartsgui_kde.la

check_PROGRAMS = artstestgui dbtest

INCLUDES= -I$(top_builddir)/arts/runtime -I$(top_builddir)/arts/gui/common -I$(arts_includes) $(all_includes)

libartsgui_kde_la_SOURCES = kwidget_impl.cpp kbutton_impl.cpp \
  kpoti_impl.cpp kpoti.cpp kwidgetrepo.cpp kartswidget.cpp \
  klineedit_impl.cpp khbox_impl.cpp kvbox_impl.cpp kspinbox_impl.cpp \
  kcombobox_impl.cpp kfader.cpp kfader_impl.cpp kgraph_impl.cpp kgraph.cpp \
  kgraphline_impl.cpp kframe_impl.cpp klayoutbox_impl.cpp kpopupbox_impl.cpp \
  klevelmeter_impl.cpp klevelmeter_small.cpp klevelmeter_linebars.cpp \
  klevelmeter_normalbars.cpp klevelmeter_firebars.cpp \
  ktickmarks_impl.cpp kvolumefader_impl.cpp klabel_impl.cpp

libartsgui_kde_la_LIBADD = $(LIB_KIO) -lkdecore -lkdeui \
  $(top_builddir)/arts/gui/common/libartsgui_idl.la
libartsgui_kde_la_LDFLAGS = $(all_libraries) -no-undefined
libartsgui_kde_la_COMPILE_FIRST = ../common/artsgui.h

artstestgui_SOURCES = artstestgui.cpp
artstestgui_LDADD = -lqtmcop -lkdecore -lartsflow \
  $(top_builddir)/arts/gui/common/libartsgui_idl.la
artstestgui_LDFLAGS	= $(all_libraries) $(KDE_RPATH)

dbtest_SOURCES = dbtest.cpp
dbtest_LDADD = -lqtmcop -lkdecore -lartsflow \
	$(top_builddir)/arts/gui/kde/libartsgui_kde.la
dbtest_LDFLAGS = $(all_libraries) $(KDE_RPATH)

libartsgui_kde_la_METASOURCES = AUTO

####### install header files

artsincludedir = $(includedir)/arts
artsinclude_HEADERS = kartswidget.h kframe_impl.h kwidget_impl.h kwidgetrepo.h klayoutbox_impl.h
