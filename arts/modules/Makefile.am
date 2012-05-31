####### Various modules for artsmodules

SUBDIRS = synth common effects mixers
INCLUDES= $(ARTSC_INCLUDE) \
	-I$(top_srcdir)/arts/modules \
	-I$(top_builddir)/arts/modules/synth \
	-I$(top_builddir)/arts/modules/common \
	-I$(top_builddir)/arts/modules/effects \
	-I$(top_builddir)/arts/modules/mixers \
	-I$(top_builddir)/arts/runtime \
	-I$(top_builddir)/arts/midi \
	-I$(top_srcdir)/arts/midi \
	-I$(top_srcdir)/arts/gui/common -I$(top_builddir)/arts/gui/common \
	-I$(arts_includes) $(all_includes)

MCOPIDLINCLUDES = \
	-I$(top_srcdir)/arts/midi \
	-I$(top_srcdir)/arts/gui/common \
	-I$(top_srcdir)/arts/modules \
	-I$(top_srcdir)/arts/modules/synth \
	-I$(top_srcdir)/arts/modules/common \
	-I$(top_srcdir)/arts/modules/effects \
	-I$(top_srcdir)/arts/modules/mixers \
	-I$(arts_includes) $(all_includes)

lib_LTLIBRARIES = libartsmodules.la

libartsmodules_la_SOURCES = artsmodules.cc

libartsmodules_la_LIBADD = \
   $(top_builddir)/arts/runtime/libartsbuilder.la \
   $(top_builddir)/arts/midi/libartsmidi_idl.la \
   $(top_builddir)/arts/gui/common/libartsgui_idl.la \
   $(top_builddir)/arts/modules/synth/libartsmodulessynth.la \
   $(top_builddir)/arts/modules/common/libartsmodulescommon.la \
   $(top_builddir)/arts/modules/effects/libartsmoduleseffects.la \
   $(top_builddir)/arts/modules/mixers/libartsmodulesmixers.la \
   -lartsflow -lmcop $(LIB_KDECORE) $(LIBDL)
libartsmodules_la_LDFLAGS = $(all_libraries) -L$(arts_libraries) \
   -no-undefined


artsmodules.mcopclass: artsmodules.h
artsmodules.mcoptype: artsmodules.h
artsmodules.cc artsmodules.h: $(srcdir)/artsmodules.idl $(MCOPIDL)
	$(MCOPIDL) -t $(MCOPIDLINCLUDES) $(srcdir)/artsmodules.idl

DISTCLEANFILES = artsmodules.cc artsmodules.h \
  artsmodules.mcoptype artsmodules.mcopclass

####### install idl files

artsincludedir = $(includedir)/arts
artsinclude_HEADERS = artsmodules.h artsmodules.idl

mcoptypedir = $(libdir)/mcop
mcoptype_DATA = artsmodules.mcoptype artsmodules.mcopclass

artsmodules.lo: artsmodules.h ../midi/artsmidi.h ../gui/common/artsgui.h common/artsmodulescommon.h synth/artsmodulessynth.h effects/artsmoduleseffects.h mixers/artsmodulesmixers.h

