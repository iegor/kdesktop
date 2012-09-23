
INCLUDES = \
	-I$(top_builddir)/arts/modules \
	-I$(top_srcdir)/arts/modules \
	-I$(top_builddir)/arts/gui/common \
	-I$(top_builddir)/arts/midi \
	-I$(top_builddir)/arts/runtime \
	-I$(arts_includes) \
	$(ARTSC_INCLUDE) $(all_includes)

MCOP_INCLUDES= \
	-I$(top_srcdir)/arts/gui/common \
	-I$(top_srcdir)/arts/midi \
	-I$(arts_includes)

lib_LTLIBRARIES = libartsmodulessynth.la

libartsmodulessynth_la_SOURCES = artsmodulessynth.cc \
	synth_xfade_impl.cc synth_autopanner_impl.cc synth_delay_impl.cc synth_cdelay_impl.cc \
	synth_envelope_adsr_impl.cc synth_pscale_impl.cc \
	synth_tremolo_impl.cc synth_fx_cflanger_impl.cc synth_compressor_impl.cc \
	synth_pitch_shift_impl.cc synth_pitch_shift_fft_impl.cc c_filter_stuff.c synth_shelve_cutoff_impl.cc synth_brickwall_limiter_impl.cc synth_std_equalizer_impl.cc synth_rc_impl.cc synth_moog_vcf_impl.cc synth_atan_saturate_impl.cc \
	synth_fm_source_impl.cc \
	synth_wave_tri_impl.cc synth_noise_impl.cc synth_wave_softsaw_impl.cc synth_wave_square_impl.cc synth_wave_pulse_impl.cc synth_osc_impl.cc synth_play_pat_impl.cc \
	synth_capture_wav_impl.cc \
	synth_midi_test_impl.cc synth_sequence_impl.cc synth_sequence_freq_impl.cc \
	synth_midi_debug_impl.cc objectcache_impl.cc synth_nil_impl.cc synth_debug_impl.cc synth_data_impl.cc \
	synth_div_impl.cc
libartsmodulessynth_la_COMPILE_FIRST = artsmodulessynth.h ../../midi/artsmidi.h

libartsmodulessynth_la_LIBADD = \
	$(top_builddir)/arts/runtime/libartsbuilder.la \
	$(top_builddir)/arts/midi/libartsmidi_idl.la \
	-lartsflow -lartsflow_idl -lmcop

#libartsmodulessynth_la_LIBADD = $(top_builddir)/arts/gui/common/libartsgui_idl.la $(top_builddir)/arts/midi/libartsmidi_idl.la -lartsflow -lartsflow_idl -lmcop $(LIBDL) $(LIB_KDEUI) $(LIB_KDECORE)
libartsmodulessynth_la_LDFLAGS = $(all_libraries) -L$(arts_libraries) -no-undefined

artsmodulessynth.cc artsmodulessynth.h artsmodulessynth.mcoptype artsmodulessynth.mcopclass: $(srcdir)/artsmodulessynth.idl $(MCOPIDL)
	$(MCOPIDL) -t $(MCOP_INCLUDES) $(srcdir)/artsmodulessynth.idl

DISTCLEANFILES= artsmodulessynth.cc artsmodulessynth.h artsmodulessynth.mcop*

artsincludedir = $(includedir)/arts
artsinclude_HEADERS = artsmodulessynth.h artsmodulessynth.idl

mcoptypedir = $(libdir)/mcop
mcoptype_DATA = artsmodulessynth.mcoptype artsmodulessynth.mcopclass

mcopclassdir = $(libdir)/mcop/Arts
mcopclass_DATA = \
	mcopclass/Synth_XFADE.mcopclass mcopclass/Synth_AUTOPANNER.mcopclass mcopclass/Synth_DELAY.mcopclass mcopclass/Synth_CDELAY.mcopclass \
	mcopclass/Synth_ENVELOPE_ADSR.mcopclass mcopclass/Synth_PSCALE.mcopclass \
	mcopclass/Synth_TREMOLO.mcopclass mcopclass/Synth_FX_CFLANGER.mcopclass mcopclass/Synth_COMPRESSOR.mcopclass \
	mcopclass/Synth_PITCH_SHIFT.mcopclass mcopclass/Synth_PITCH_SHIFT_FFT.mcopclass mcopclass/Synth_SHELVE_CUTOFF.mcopclass mcopclass/Synth_BRICKWALL_LIMITER.mcopclass mcopclass/Synth_STD_EQUALIZER.mcopclass mcopclass/Synth_RC.mcopclass mcopclass/Synth_MOOG_VCF.mcopclass mcopclass/Synth_ATAN_SATURATE.mcopclass \
	mcopclass/Synth_FM_SOURCE.mcopclass \
	mcopclass/Synth_WAVE_TRI.mcopclass mcopclass/Synth_NOISE.mcopclass mcopclass/Synth_WAVE_SOFTSAW.mcopclass mcopclass/Synth_WAVE_SQUARE.mcopclass mcopclass/Synth_WAVE_PULSE.mcopclass mcopclass/Synth_OSC.mcopclass mcopclass/Synth_PLAY_PAT.mcopclass \
	mcopclass/Synth_CAPTURE_WAV.mcopclass mcopclass/Synth_DIV.mcopclass \
	mcopclass/Synth_MIDI_TEST.mcopclass mcopclass/Synth_SEQUENCE.mcopclass \
	mcopclass/Synth_SEQUENCE_FREQ.mcopclass \
	mcopclass/Synth_MIDI_DEBUG.mcopclass mcopclass/Synth_DATA.mcopclass mcopclass/Synth_DEBUG.mcopclass mcopclass/Synth_NIL.mcopclass

synth_midi_test_impl.lo: ../../runtime/artsbuilder.h

