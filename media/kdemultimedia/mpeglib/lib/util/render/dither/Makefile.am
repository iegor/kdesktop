#  liboutplugin - Makefile.am

INCLUDES                =       $(all_includes)

EXTRA_DIST		=	ditherDef.h ditherMMX.h \
				ditherer_mmx16.cpp dither32mmx.cpp

noinst_LTLIBRARIES	=	libdither.la

noinst_HEADERS		=	ditherWrapper.h \
				dither8Bit.h colorTable8Bit.h \
				colorTableHighBit.h dither16Bit.h \
				dither32Bit.h ditherRGB_flipped.h \
				ditherRGB.h

libdither_la_SOURCES	=	ditherWrapper.cpp \
				dither8Bit.cpp \
				colorTable8Bit.cpp colorTableHighBit.cpp \
				dither16Bit.cpp dither32Bit.cpp \
				ditherRGB_flipped.cpp ditherRGB.cpp \
				ditherer_mmx16.cpp dither32mmx.cpp



















