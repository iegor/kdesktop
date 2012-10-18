# $Id: Makefile.am 158249 2002-05-28 12:59:17Z coolo $

INCLUDES= -I$(kde_includes)/arts $(all_includes)

noinst_HEADERS = oggPlayObject_impl.h

lib_LTLIBRARIES = liboggarts.la
liboggarts_la_COMPILE_FIRST = oggarts.h
liboggarts_la_SOURCES = oggarts.cc oggPlayObject_impl.cpp
liboggarts_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
liboggarts_la_LIBADD =  -lkmedia2_idl -lsoundserver_idl -lartsflow -lvorbisfile -lvorbis -logg
liboggarts_la_METASOURCES = AUTO

oggarts.mcopclass: oggarts.h
oggarts.mcoptype: oggarts.h
oggarts.cc oggarts.h: $(srcdir)/oggarts.idl $(MCOPIDL)
	$(MCOPIDL) -t -I$(kde_includes)/arts $(srcdir)/oggarts.idl

mcoptypedir = $(libdir)/mcop
mcoptype_DATA = oggarts.mcoptype oggarts.mcopclass

mcopclassdir = $(libdir)/mcop/Arts
mcopclass_DATA = oggPlayObject.mcopclass
