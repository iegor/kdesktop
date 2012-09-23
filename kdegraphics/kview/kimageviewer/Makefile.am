# $Id: Makefile.am 206066 2003-02-10 17:18:41Z mkretz $

lib_LTLIBRARIES = libkimageviewer.la

noinst_HEADERS = canvas.h viewer.h

libkimageviewer_la_SOURCES = canvas.cpp viewer.cpp
libkimageviewer_la_LDFLAGS = $(all_libraries) -no-undefined -version-info 0:0
libkimageviewer_la_LIBADD  = $(LIB_KPARTS)

INCLUDES = $(all_includes)

# kimageviewerinclude_HEADERS = canvas.h viewer.h
# kimageviewerincludedir = $(includedir)/kimageviewer

METASOURCES = AUTO

kde_servicetypes_DATA = kimageviewercanvas.desktop kimageviewer.desktop
