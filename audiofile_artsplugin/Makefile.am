INCLUDES= -I$(kde_includes)/arts $(all_includes)

noinst_HEADERS = audiofilePlayObjectI.h

lib_LTLIBRARIES = libarts_audiofile.la
libarts_audiofile_la_COMPILE_FIRST = audiofilearts.h
libarts_audiofile_la_SOURCES = audiofilearts.cc audiofilePlayObjectI.cpp
libarts_audiofile_la_LDFLAGS = $(all_libraries) -module -no-undefined
libarts_audiofile_la_LIBADD =  -lkmedia2_idl -lsoundserver_idl -lartsflow -laudiofile
libarts_audiofile_la_METASOURCES = AUTO

audiofilearts.mcopclass: audiofilearts.h
audiofilearts.mcoptype: audiofilearts.h
audiofilearts.cc audiofilearts.h: $(srcdir)/audiofilearts.idl $(MCOPIDL)
	$(MCOPIDL) -t -I$(kde_includes)/arts $(srcdir)/audiofilearts.idl

mcoptypedir = $(libdir)/mcop
mcoptype_DATA = audiofilearts.mcoptype audiofilearts.mcopclass

mcopclassdir = $(libdir)/mcop/Arts
mcopclass_DATA = audiofilePlayObject.mcopclass

CLEANFILES=audiofilearts.h audiofilearts.cc audiofilearts.mcopclass audiofilearts.mcoptype
