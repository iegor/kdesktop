
SUBDIRS = freeverb

INCLUDES = \
	-I$(top_builddir)/arts/modules/effects \
	-I$(top_srcdir)/arts/modules/effects \
	-I$(top_builddir)/arts/modules/synth \
	-I$(top_srcdir)/arts/modules/synth \
	-I$(top_builddir)/arts/modules/common \
	-I$(top_srcdir)/arts/modules/common \
	-I$(top_builddir)/arts/modules \
	-I$(top_srcdir)/arts/modules \
	-I$(top_builddir)/arts/gui/common \
	-I$(top_srcdir)/arts/gui/common \
	-I$(top_srcdir)/arts/gui/kde \
	-I$(top_builddir)/arts/midi \
	-I$(top_srcdir)/arts/midi \
	-I$(arts_includes) \
	$(all_includes)

lib_LTLIBRARIES = libartsmoduleseffects.la

libartsmoduleseffects_la_SOURCES = artsmoduleseffects.cc \
	fivebandmonocomplexeq_impl.cc \
	monostereoconversion_impl.cc \
	synth_stereo_pitch_shift_impl.cc synth_stereo_pitch_shift_fft_impl.cc \
	synth_voice_removal_impl.cc voiceremovalguifactory_impl.cc \
	synth_stereo_compressor_impl.cc stereocompressorguifactory_impl.cc \
	synth_stereo_fir_equalizer_impl.cc \
	synth_freeverb_impl.cc freeverbguifactory_impl.cc \
	effect_wavecapture_impl.cc \
	kstereovolumecontrolgui_impl.cpp stereovolumecontrolguifactory_impl.cpp
libartsmoduleseffects_la_COMPILE_FIRST = ../../gui/common/artsgui.h \
	../common/artsmodulescommon.h ../../midi/artsmidi.h ../synth/artsmodulessynth.h \
	artsmoduleseffects.h
libartsmoduleseffects_la_LIBADD = \
	$(top_builddir)/arts/modules/effects/freeverb/libfreeverb.la \
	$(top_builddir)/arts/gui/common/libartsgui_idl.la \
	$(top_builddir)/arts/gui/kde/libartsgui_kde.la \
	$(top_builddir)/arts/modules/common/libartsmodulescommon.la \
	-lartsflow -lartsflow_idl -lmcop

libartsmoduleseffects_la_LDFLAGS = $(all_libraries) -L$(arts_libraries) -no-undefined

METASOURCES=AUTO

artsmoduleseffects.cc artsmoduleseffects.h artsmoduleseffects.mcoptype artsmoduleseffects.mcopclass: $(srcdir)/artsmoduleseffects.idl $(MCOPIDL)
	$(MCOPIDL) -t $(INCLUDES) $(srcdir)/artsmoduleseffects.idl

DISTCLEANFILES= artsmoduleseffects.cc artsmoduleseffects.h artsmoduleseffects.mcop*

artsincludedir = $(includedir)/arts
artsinclude_HEADERS = artsmoduleseffects.h artsmoduleseffects.idl

mcoptypedir = $(libdir)/mcop
mcoptype_DATA = artsmoduleseffects.mcoptype artsmoduleseffects.mcopclass

mcopclassdir = $(libdir)/mcop/Arts
mcopclass_DATA = \
	mcopclass/FiveBandMonoComplexEQ.mcopclass mcopclass/FiveBandMonoComplexEQGuiFactory.mcopclass \
	mcopclass/MonoToStereo.mcopclass mcopclass/StereoToMono.mcopclass \
	mcopclass/StereoBalance.mcopclass mcopclass/StereoBalanceGuiFactory.mcopclass \
	mcopclass/Synth_VOICE_REMOVAL.mcopclass mcopclass/VoiceRemovalGuiFactory.mcopclass \
	mcopclass/Synth_STEREO_COMPRESSOR.mcopclass mcopclass/StereoCompressorGuiFactory.mcopclass \
	mcopclass/Synth_STEREO_PITCH_SHIFT.mcopclass mcopclass/Synth_STEREO_PITCH_SHIFT_FFT.mcopclass \
	mcopclass/Synth_STEREO_FIR_EQUALIZER.mcopclass mcopclass/StereoFirEqualizerGuiFactory.mcopclass \
	mcopclass/Synth_FREEVERB.mcopclass mcopclass/FreeverbGuiFactory.mcopclass \
	mcopclass/Effect_WAVECAPTURE.mcopclass \
	mcopclass/StereoVolumeControlGui.mcopclass mcopclass/StereoVolumeControlGuiFactory.mcopclass

