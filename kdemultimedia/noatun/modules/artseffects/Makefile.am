INCLUDES= -I$(top_builddir)/arts/gui/common -I$(top_srcdir)/arts/gui/common -I$(kde_includes)/arts $(all_includes)
lib_LTLIBRARIES = libartseffects.la
#libartseffectsui.la

libartseffects_la_SOURCES = artseffects.cc extrastereo_impl.cc
libartseffects_la_COMPILE_FIRST = artseffects.cc
libartseffects_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libartseffects_la_LIBADD =  $(top_builddir)/arts/gui/common/libartsgui_idl.la -lkmedia2_idl -lsoundserver_idl -lartsflow


artseffects.mcopclass: artseffects.h
artseffects.mcoptype: artseffects.h
artseffects.cc artseffects.h: $(srcdir)/artseffects.idl $(MCOPIDL)
	$(MCOPIDL) -t -I$(top_builddir)/arts/gui/common -I$(top_srcdir)/arts/gui/common -I$(kde_includes)/arts $(srcdir)/artseffects.idl

CLEANFILES = artsmidi.cc artsmidi.h artsmidi.mcoptype artsmidi.mcopclass

mcoptypedir = $(libdir)/mcop
mcoptype_DATA = artseffects.mcoptype artseffects.mcopclass

mcopclassdir = $(libdir)/mcop
mcopclass_DATA = ExtraStereo.mcopclass VoiceRemoval.mcopclass RawWriter.mcopclass ExtraStereoGuiFactory.mcopclass

artseffects.lo: artseffects.h ../../../arts/gui/common/artsgui.h
extrastereo_impl.lo: ../../../arts/gui/common/artsgui.h artseffects.h
