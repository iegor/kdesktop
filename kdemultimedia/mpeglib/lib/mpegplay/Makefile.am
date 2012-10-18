#  libsplay - Makefile.am

EXTRA_DIST		=	mainMpegPlay.cpp README COPYRIGHT \
				mmxidct_asm.S copyFunctions.cpp

INCLUDES                =       $(all_includes)

AM_ASFLAGS = $(DEFS) $(DEFAULT_INCLUDES) $(all_includes) $(INTELCPPFLAG)
AM_CCASFLAGS = $(AM_ASFLAGS)


noinst_LTLIBRARIES	=	libmpegplay.la 

noinst_HEADERS		=	mpegVideoHeader.h mpegVideoStream.h \
				decoderTables.h gop.h \
				proto.h \
				recon.h  startCodes.h \
				jrevdct.h  \
				motionVector.h slice.h \
				decoderClass.h \
				mmxidct.h \
				picture.h mpegExtension.h macroBlock.h \
				copyFunctions.h \
				mpegVideoBitWindow.h videoDecoder.h \
				copyFunctions_asm.h copyFunctions_mmx.h 

mpegutildir		=	$(includedir)/$(THIS_LIB_NAME)/mpegutil

mpegutil_HEADERS	=	mpegSystemStream.h mpegVideoLength.h \
				mpegSystemHeader.h tsSystemStream.h \
				psSystemStream.h pesSystemStream.h

libmpegplay_la_SOURCES	=	mpegVideoHeader.cpp mpegVideoStream.cpp \
				globals.cpp jrevdct.cpp \
				recon.cpp decoderClass.cpp \
				decoderTables.cpp motionVector.cpp \
				slice.cpp gop.cpp \
				mmxidct.cpp \
				mpegSystemHeader.cpp mpegSystemStream.cpp \
				picture.cpp mpegExtension.cpp \
				macroBlock.cpp  \
				mpegVideoLength.cpp \
				mpegVideoBitWindow.cpp videoDecoder.cpp \
				copyFunctions_asm.cpp copyFunctions_mmx.cpp \
				mmxidct_asm.S copyFunctions.cpp \
				tsSystemStream.cpp psSystemStream.cpp \
				pesSystemStream.cpp

