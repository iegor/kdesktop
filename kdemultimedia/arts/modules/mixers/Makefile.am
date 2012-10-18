
INCLUDES = \
	-I$(top_builddir)/arts/modules \
	-I$(top_srcdir)/arts/modules \
	-I$(top_builddir)/arts/modules/common \
	-I$(top_srcdir)/arts/modules/common \
	-I$(top_builddir)/arts/modules/synth \
	-I$(top_srcdir)/arts/modules/synth \
	-I$(top_builddir)/arts/modules/effects \
	-I$(top_builddir)/arts/gui/common \
	-I$(top_srcdir)/arts/gui/common \
	-I$(top_builddir)/arts/midi \
	-I$(top_srcdir)/arts/midi \
	-I$(arts_includes) \
	$(all_includes)

MCOPINCLUDES = \
	-I$(top_srcdir)/arts/modules \
	-I$(top_srcdir)/arts/modules/common \
	-I$(top_srcdir)/arts/modules/synth \
	-I$(top_srcdir)/arts/gui/common \
	-I$(top_srcdir)/arts/midi \
	-I$(arts_includes) \
	$(all_includes)

lib_LTLIBRARIES = libartsmodulesmixers.la

libartsmodulesmixers_la_SOURCES = artsmodulesmixers.cc \
	monosimplemixerchannel_impl.cc monosimplemixerchannelguifactory_impl.cc \
	simplemixerchannel_impl.cc simplemixerchannelguifactory_impl.cc \
	littlestereomixerchannel_impl.cc
libartsmodulesmixers_la_COMPILE_FIRST = ../synth/artsmodulessynth.h \
	../../midi/artsmidi.h ../common/artsmodulescommon.h ../../gui/common/artsgui.h \
	artsmodulesmixers.h
libartsmodulesmixers_la_LIBADD = \
	$(top_builddir)/arts/gui/common/libartsgui_idl.la \
	$(top_builddir)/arts/modules/common/libartsmodulescommon.la \
	$(top_builddir)/arts/modules/effects/libartsmoduleseffects.la \
	-lartsflow -lartsflow_idl -lmcop

libartsmodulesmixers_la_LDFLAGS = $(all_libraries) -L$(arts_libraries) -no-undefined

artsmodulesmixers.cc artsmodulesmixers.h artsmodulesmixers.mcoptype artsmodulesmixers.mcopclass: $(srcdir)/artsmodulesmixers.idl $(MCOPIDL)
	$(MCOPIDL) -t $(MCOPINCLUDES) $(srcdir)/artsmodulesmixers.idl

DISTCLEANFILES= artsmodulesmixers.cc artsmodulesmixers.h artsmodulesmixers.mcop*

artsincludedir = $(includedir)/arts
artsinclude_HEADERS = artsmodulesmixers.h artsmodulesmixers.idl

mcoptypedir = $(libdir)/mcop
mcoptype_DATA = artsmodulesmixers.mcoptype artsmodulesmixers.mcopclass

mcopclassdir = $(libdir)/mcop/Arts
mcopclass_DATA = \
	mcopclass/MonoSimpleMixerChannel.mcopclass mcopclass/MonoSimpleMixerChannelGuiFactory.mcopclass  \
	mcopclass/SimpleMixerChannel.mcopclass mcopclass/SimpleMixerChannelGuiFactory.mcopclass \
	mcopclass/LittleStereoMixerChannel.mcopclass mcopclass/LittleStereoMixerChannelGuiFactory.mcopclass

littlestereomixerchannel_impl.lo: ../effects/artsmoduleseffects.h
