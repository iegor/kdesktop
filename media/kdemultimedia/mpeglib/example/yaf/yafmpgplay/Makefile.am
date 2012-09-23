#  mpegplay-yaf - Makefile.am

INCLUDES		=	-I../../include $(all_includes)

EXTRA_DIST		=	stresstest

bin_PROGRAMS		=	yaf-mpgplay

yaf_mpgplay_SOURCES	=	mpg_control.cpp

noinst_HEADERS		=	


yaf_mpgplay_LDFLAGS	=	$(all_libraries) $(KDE_RPATH) 


yaf_mpgplay_LDADD	=	../yafcore/libyafcore.la \
				../yafxplayer/libyafxplayer.la \
				../../../lib/libmpeg.la \
				$(THIS_LIB_LIBS)




				









