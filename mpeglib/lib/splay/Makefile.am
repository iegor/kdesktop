#  libsplay - Makefile.am


INCLUDES                =       $(all_includes)


EXTRA_DIST		=	dct64.cpp dct64_down.cpp dct36_12.cpp \
				window.cpp dct.h


noinst_LTLIBRARIES	=	libsplay.la

noinst_HEADERS		=	mpegAudioHeader.h mpegAudioStream.h \
				mpegsound.h op.h \
				sigsev.c dump.h \
				dxHead.h mpeg2tables.h \
				mpegAudioBitWindow.h huffmanlookup.h \
				common.h attribute.h synthesis.h

kmpgincludedir          =       $(includedir)/$(THIS_LIB_NAME)/splay

kmpginclude_HEADERS     =	splayDecoder.h mpegAudioInfo.h \
				mpegAudioFrame.h


libsplay_la_SOURCES	=	mpegAudioHeader.cpp mpegAudioStream.cpp \
				huffmantable.cpp \
				mpeglayer1.cpp \
				mpeglayer2.cpp \
				mpeglayer3.cpp \
				mpegtable.cpp \
				mpegtoraw.cpp \
				dxHead.cpp \
				mpegAudioBitWindow.cpp huffmanlookup.cpp \
				splayDecoder.cpp \
				dump.cpp synth_filter.cpp \
				synthesis.cpp synth_Std.cpp synth_Down.cpp \
				mpegAudioFrame.cpp \
				mpegAudioInfo.cpp



#CXXFLAGS += -fno-strength-reduce 
#		-funroll-all-loops    -finline-functions -ffast-math -m486









