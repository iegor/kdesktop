####### Various modules for artsmodules

SUBDIRS = mcopclass
INCLUDES= -I$(top_builddir)/arts/runtime -I$(srcdir)/freeverb -I$(arts_includes) $(all_includes)

lib_LTLIBRARIES = libartsmidi_idl.la libartsmidi.la

bin_PROGRAMS = midisend
noinst_PROGRAMS = midisynctest

midisend_SOURCES = midisend.cc midimsg.c
midisend_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(LIBPTHREAD)
midisend_LDADD   = libartsmidi_idl.la
midisend_COMPILE_FIRST = artsmidi.h

midisynctest_SOURCES = midisynctest.cc
midisynctest_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(LIBPTHREAD)
midisynctest_LDADD   = libartsmidi.la -lsoundserver_idl
midisynctest_COMPILE_FIRST = artsmidi.h

libartsmidi_idl_la_SOURCES = artsmidi.cc
libartsmidi_idl_la_LIBADD = -lmcop -lartsflow
libartsmidi_idl_la_LDFLAGS = $(all_libraries) -L$(arts_libraries) \
  -no-undefined

libartsmidi_la_SOURCES = midimanager_impl.cc midiclient_impl.cc \
	miditest_impl.cc midimanagerport_impl.cc rawmidiport_impl.cc \
	systemmiditimer_impl.cc audiomiditimer_impl.cc miditimercommon.cc \
	audiosync_impl.cc audiotimer.cc alsamidigateway_impl.cc \
	alsamidiport_impl.cc midisyncgroup_impl.cc timestampmath.cc
libartsmidi_la_COMPILE_FIRST = artsmidi.h

libartsmidi_la_LIBADD = libartsmidi_idl.la -lartsflow $(ARTS_LIBASOUND)
libartsmidi_la_LDFLAGS = $(all_libraries) -L$(arts_libraries) \
  -no-undefined

artsmidi.mcopclass: artsmidi.h
artsmidi.mcoptype: artsmidi.h
artsmidi.cc artsmidi.h: $(srcdir)/artsmidi.idl $(MCOPIDL)
	$(MCOPIDL) -t -I$(arts_includes) $(srcdir)/artsmidi.idl

DISTCLEANFILES = artsmidi.cc artsmidi.h \
  artsmidi.mcoptype artsmidi.mcopclass

####### install idl files

artsincludedir = $(includedir)/arts
artsinclude_HEADERS = artsmidi.h artsmidi.idl

mcoptypedir = $(libdir)/mcop
mcoptype_DATA = artsmidi.mcoptype artsmidi.mcopclass

artsmidi.lo: artsmidi.h
