#  player - Makefile.am

SUBDIRS				=	dither dither2YUV x11 sdl

INCLUDES                	=       $(all_includes)

THIS_EXTRALIBS			=	dither/libdither.la \
					dither2YUV/libdivxutil_dither.la \
					x11/libutilx11.la


noinst_LTLIBRARIES		=	libutilrender.la

noinst_HEADERS			=	imageBase.h \
					renderMachine.h surface.h 


kmpgincludedir                  =       $(includedir)/$(THIS_LIB_NAME)/util/render

kmpginclude_HEADERS             =       yuvPicture.h pictureArray.h


libutilrender_la_SOURCES	=	yuvPicture.cpp imageBase.cpp \
					renderMachine.cpp surface.cpp \
					pictureArray.cpp

libutilrender_la_LIBADD		=	$(THIS_EXTRALIBS)
































