#  player - Makefile.am

SUBDIRS		=	frame util input output decoder \
			splay oggvorbis mpegplay mpgplayer tplay yuv

MMXFILES	=	


EXTRA_DIST	=	README


THIS_EXTRALIBS	=	mpgplayer/libmpgplayer.la \
			mpegplay/libmpegplay.la \
			splay/libsplay.la \
			oggvorbis/liboggvorbisbase.la \
			tplay/libtplay.la \
			decoder/libdecoder.la \
			output/liboutput.la \
			input/libinput.la \
			util/libutil.la \
			frame/libframe.la \
			yuv/libyuvPlugin.la



lib_LTLIBRARIES		=	libmpeg.la

libmpeg_la_SOURCES	=	dummy.cpp


libmpeg_la_LDFLAGS	=	$(all_libraries) \
				-release $(THIS_LIB_VERSION)


libmpeg_la_LIBADD	=	$(THIS_EXTRALIBS) $(MMXFILES) \
				$(THIS_LIB_LIBS)
