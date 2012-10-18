INCLUDES= -I$(kde_includes)/arts $(all_includes)
KDE_OPTIONS = nofinal

lib_LTLIBRARIES = libnoatunarts.la
libnoatunarts_la_SOURCES = noatunarts.cc fft.c Equalizer_impl.cpp \
                           FFTScopes.cpp StereoEffectStack_impl.cpp \
                           StereoVolumeControl_impl.cpp Session_impl.cpp
libnoatunarts_la_COMPILE_FIRST = noatunarts.h
libnoatunarts_la_LDFLAGS = $(all_libraries) -avoid-version -no-undefined
libnoatunarts_la_LIBADD =  -lkmedia2_idl -lsoundserver_idl -lartsflow
libnoatunarts_la_METASOURCES = AUTO

noatunarts.mcoptype: noatunarts.h
noatunarts.mcopclass: noatunarts.h

noatunarts.cc noatunarts.h: noatunarts.idl 
	$(MCOPIDL) -t -I$(kde_includes)/arts $(srcdir)/noatunarts.idl

mcoptypedir = $(libdir)/mcop
mcoptype_DATA = noatunarts.mcoptype noatunarts.mcopclass

mcopclassdir = $(libdir)/mcop/Noatun
mcopclass_DATA = Equalizer.mcopclass FFTScopeStereo.mcopclass StereoEffectStack.mcopclass \
                 EqualizerSSE.mcopclass RawScope.mcopclass StereoVolumeControl.mcopclass \
                 FFTScope.mcopclass RawScopeStereo.mcopclass StereoVolumeControlSSE.mcopclass \
				 Session.mcopclass Listener.mcopclass

noatuninclude_HEADERS= noatunarts.h

noatunincludedir = $(includedir)/noatun

DISTCLEANFILES = noatunarts.cc noatunarts.h noatunarts.mcopclass noatunarts.mcoptype

