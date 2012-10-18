#  libxplayer - Makefile.am

INCLUDES			=	-I../../include $(all_includes)

AM_CPPFLAGS			=	-DDIRECT_INCLUDE

EXTRA_DIST			=	xplayerCommand.defs  xplayerRuntime.defs

lib_LTLIBRARIES			=	libyafxplayer.la


noinst_HEADERS			=	commandTableXPlayer.h  runtimeTableXPlayer.h \
					xplayer_control.h


libyafxplayer_la_SOURCES	=	commandTableXPlayer.cpp \
					inputDecoderXPlayer.cpp \
					xplayer_control.cpp \
					runtimeTableXPlayer.cpp \
					inputDecoderYAF.cpp yafOutputStream.cpp


libyafxplayer_la_LDFLAGS	=	$(all_libraries) $(KDE_RPATH) \
					-no-undefined -avoid-version

libyafxplayer_la_LIBADD		=	../../../lib/libmpeg.la \
					../yafcore/libyafcore.la \
					$(THIS_LIB_LIBS)



yafxplayerdir			=	$(includedir)/$(THIS_LIB_NAME)/util/yaf/yafxplayer

yafxplayer_HEADERS		=	inputDecoderYAF.h \
					inputDecoderXPlayer.h \
					yafOutputStream.h

