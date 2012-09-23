# $Id: Makefile.am 158249 2002-05-28 12:59:17Z coolo $

INCLUDES= -I$(kde_includes)/arts $(all_includes)

noinst_HEADERS = mpg123PlayObject_impl.h

lib_LTLIBRARIES = libmpg123arts.la
libmpg123arts_la_COMPILE_FIRST = mpg123arts.h
libmpg123arts_la_SOURCES = mpg123arts.cc mpg123PlayObject_impl.cpp dxhead.c
libmpg123arts_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libmpg123arts_la_LIBADD =  -lkmedia2_idl -lsoundserver_idl -lartsflow mpg123/libmpg123.la
libmpg123arts_la_METASOURCES = AUTO

mpg123arts.mcoptype: mpg123arts.h
mpg123arts.mcopclass: mpg123arts.h
mpg123arts.cc mpg123arts.h: $(srcdir)/mpg123arts.idl $(MCOPIDL)
	$(MCOPIDL) -t -I$(kde_includes)/arts $(srcdir)/mpg123arts.idl

mcoptypedir = $(libdir)/mcop
mcoptype_DATA = mpg123arts.mcoptype mpg123arts.mcopclass

mcopclassdir = $(libdir)/mcop/Arts
mcopclass_DATA = mpg123PlayObject.mcopclass

SUBDIRS = mpg123 .
