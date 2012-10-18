
INCLUDES = \
	-I$(top_builddir)/arts/modules/common \
	-I$(top_builddir)/arts/modules/synth \
	-I$(top_srcdir)/arts/modules/synth \
	-I$(top_builddir)/arts/modules \
	-I$(top_srcdir)/arts/modules \
	-I$(top_builddir)/arts/gui/common \
	-I$(top_srcdir)/arts/gui/common \
	-I$(top_builddir)/arts/midi \
	-I$(top_srcdir)/arts/midi \
	-I$(arts_includes) \
	$(all_includes)

lib_LTLIBRARIES = libartsmodulescommon.la

libartsmodulescommon_la_SOURCES = artsmodulescommon.cc \
	effectrackslot_impl.cc env_container_impl.cc \
	env_context_impl.cc env_effectrackitem_impl.cc \
	env_instrumentitem_impl.cc env_item_impl.cc \
	env_mixeritem_impl.cc
libartsmodulescommon_la_COMPILE_FIRST = artsmodulescommon.h

libartsmodulescommon_la_LIBADD = \
	$(top_builddir)/arts/gui/common/libartsgui_idl.la \
	$(top_builddir)/arts/midi/libartsmidi_idl.la \
	$(top_builddir)/arts/modules/synth/libartsmodulessynth.la \
	-lartsflow -lartsflow_idl -lmcop $(LIB_KDECORE)

libartsmodulescommon_la_LDFLAGS = $(all_libraries) -L$(arts_libraries) -no-undefined

artsmodulescommon.cc artsmodulescommon.h artsmodulescommon.mcoptype artsmodulescommon.mcopclass: $(srcdir)/artsmodulescommon.idl $(MCOPIDL)
	$(MCOPIDL) -t $(INCLUDES) $(srcdir)/artsmodulescommon.idl

DISTCLEANFILES= artsmodulescommon.cc artsmodulescommon.h artsmodulescommon.mcop*

artsincludedir = $(includedir)/arts
artsinclude_HEADERS = artsmodulescommon.h artsmodulescommon.idl

mcoptypedir = $(libdir)/mcop
mcoptype_DATA = artsmodulescommon.mcoptype artsmodulescommon.mcopclass

mcopclassdir = $(libdir)/mcop/Arts
mcopclass_DATA = \
	mcopclass/EffectRackGuiFactory.mcopclass mcopclass/MixerGuiFactory.mcopclass 

mcopclassenvdir = $(libdir)/mcop/Arts/Environment
mcopclassenv_DATA= \
	mcopclass/InstrumentItem.mcopclass mcopclass/Container.mcopclass \
	mcopclass/MixerItem.mcopclass mcopclass/EffectRackItem.mcopclass \
	mcopclass/InstrumentItemGuiFactory.mcopclass

artsmodulescommon.lo: ../../gui/common/artsgui.h ../../midi/artsmidi.h ../synth/artsmodulessynth.h
effectrackslot_impl.lo: ../../gui/common/artsgui.h ../../midi/artsmidi.h ../synth/artsmodulessynth.h
env_container_impl.lo: ../../gui/common/artsgui.h ../../midi/artsmidi.h ../synth/artsmodulessynth.h
env_context_impl.lo: ../../gui/common/artsgui.h ../../midi/artsmidi.h ../synth/artsmodulessynth.h
env_effectrackitem_impl.lo: ../../gui/common/artsgui.h ../../midi/artsmidi.h ../synth/artsmodulessynth.h
env_instrumentitem_impl.lo: ../../gui/common/artsgui.h ../../midi/artsmidi.h ../synth/artsmodulessynth.h
env_item_impl.lo: ../../gui/common/artsgui.h ../../midi/artsmidi.h ../synth/artsmodulessynth.h
env_mixeritem_impl.lo: ../../gui/common/artsgui.h ../../midi/artsmidi.h ../synth/artsmodulessynth.h

