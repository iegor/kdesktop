# This file you have to edit. Change the name here


mcopclassdir			=	$(libdir)/mcop
mcopclass_DATA			=	WAVPlayObject.mcopclass \
					MP3PlayObject.mcopclass \
					OGGPlayObject.mcopclass \
					CDDAPlayObject.mcopclass \
					NULLPlayObject.mcopclass \
					SplayPlayObject.mcopclass



EXTRA_DIST			=	doemacs  \
					configure.in.in \
					decoderBaseObject.idl \
					splayPlayObject.idl \
					$(mcopclass_DATA)


BUILT_SOURCES			=	decoderBaseObject.h \
					decoderBaseObject.cc \
					splayPlayObject.h \
					splayPlayObject.cc


noinst_HEADERS			=	mp3PlayObject_impl.h \
					oggPlayObject_impl.h \
					decoderBaseObject_impl.h \
					mpgPlayObject_impl.h \
					wavPlayObject_impl.h \
					cddaPlayObject_impl.h \
					vcdPlayObject_impl.h \
					nullPlayObject_impl.h \
					splayPlayObject_impl.h


ARTS_INCLUDES			=	$(ARTSC_INCLUDE) \
					-I$(kde_includes)/arts


INCLUDES			= 	-I../mpeglib/lib -I$(srcdir)/../mpeglib/lib $(ARTS_INCLUDES) $(all_includes)


lib_LTLIBRARIES 		=	libarts_mpeglib.la \
					libarts_splay.la

decoderBaseObject.cc decoderBaseObject.h : $(srcdir)/decoderBaseObject.idl
		$(MCOPIDL) -I$(kde_includes)/arts $(srcdir)/decoderBaseObject.idl

splayPlayObject.cc splayPlayObject.h : $(srcdir)/splayPlayObject.idl
		$(MCOPIDL) -I$(kde_includes)/arts $(srcdir)/splayPlayObject.idl

mpeglibartsplugdir		=	$(includedir)/mpeglib_artsplug

mpeglibartsplug_HEADERS		=	decoderBaseObject_impl.h \
					decoderBaseObject.h \
					decoderBaseObject.idl \
					splayPlayObject_impl.h \
					splayPlayObject.h \
					splayPlayObject.idl



libarts_mpeglib_la_SOURCES	=	decoderBaseObject.cc \
			     		splayPlayObject.cc \
					decoderBaseObject_impl.cpp \
					oggPlayObject_impl.cpp \
					mpgPlayObject_impl.cpp \
					wavPlayObject_impl.cpp \
					mp3PlayObject_impl.cpp \
					cddaPlayObject_impl.cpp \
					vcdPlayObject_impl.cpp \
					nullPlayObject_impl.cpp
libarts_mpeglib_la_COMPILE_FIRST =      decoderBaseObject.h

libarts_mpeglib_la_LDFLAGS	=	$(all_libraries) \
					-no-undefined \
					-module \
					-release $(MPEGLIB_ARTS_VERSION) \
					-version-info $(MPEGLIB_ARTS_MAJOR_VERSION):$(MPEGLIB_ARTS_MINOR_VERSION):$(MPEGLIB_ARTS_MICRO_VERSION)
libarts_mpeglib_la_LIBADD       =       $(top_builddir)/mpeglib/lib/libmpeg.la \
					$(LIB_ARTS)

libarts_splay_la_SOURCES	=	splayPlayObject.cc \
					splayPlayObject_impl.cpp

libarts_splay_la_LDFLAGS	=	$(all_libraries) \
					-no-undefined	 \
					-module
libarts_splay_la_LIBADD         =       $(top_builddir)/mpeglib/lib/libmpeg.la \
					$(LIB_ARTS)


bin_PROGRAMS			=	mpeglibartsplay

mpeglibartsplay_LDFLAGS         =       $(ARTSC_LIBS) $(all_libraries)
mpeglibartsplay_SOURCES		=	mpeglibartsplay.cpp

mpeglibartsplay_LDADD		=	-lsoundserver_idl \
					-lkmedia2_idl \
					-lartsflow_idl \
					-lmcop

splayPlayObject.lo: splayPlayObject.h
splayPlayObject_impl.lo: splayPlayObject.h
