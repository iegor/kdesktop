SUBDIRS = example
INCLUDES = $(all_includes)
METASOURCES = AUTO

lib_LTLIBRARIES = libkunittestgui.la
libkunittestgui_la_SOURCES = testerwidget.ui runnergui.cpp dcopinterface.skel
libkunittestgui_la_LDFLAGS = $(all_libraries) $(KDE_RPATH)
libkunittestgui_la_LIBADD = -lkunittest $(LIB_KDECORE)

runnergui.lo : testerwidget.h

bin_PROGRAMS = kunittestguimodrunner
kunittestguimodrunner_SOURCES = guimodrunner.cpp
kunittestguimodrunner_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kunittestguimodrunner_LDADD = libkunittestgui.la $(LIB_KDECORE) $(LIB_KIO)

noinst_HEADERS = dcopinterface.h
libkunittestinclude_HEADERS = runnergui.h
libkunittestincludedir = $(includedir)/kunittest

bin_SCRIPTS = kunittest kunittestmod kunittest_debughelper
