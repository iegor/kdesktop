# $Id: Makefile.am 169017 2002-07-27 09:29:23Z coolo $

EXTRA_DIST = README decode_i386.c decode_i586.s dct64_i386.c decode.c \
			dct64.c audio.h

# -I$(ROOT_DIR)/app 
INCLUDES = -I$(top_srcdir)/include 
COMMON_CFLAGS = -D_REENTRANT -DNOXFERMEM -DNO_EQUALIZER \
	-DNO_DECODE_AUDIO -DNO_DECODE_FILE -DNO_DECODE_WAV

libmpg123_la_SOURCES = common.c decode_2to1.c decode_4to1.c decode_ntom.c \
	layer1.c layer2.c layer3.c readers.c \
	httpget.c getbits.c tabinit.c xfermem.c vbrhead.c buffer.c audio.c

noinst_HEADERS = huffman.h mpg123.h l2tables.h audio.h common.h \
		genre.h getbits.h xfermem.h buffer.h

EXTRA_LTLIBRARIES = libmp486.la libmp586.la libmp586mmx.la libmpppc.la libmp686.la libmp686mmx.la libmpplain.la
libmp486_la_SOURCES = decode_i386.c dct64_i386.c decode_i486.c dct64_i486.c
libmp586_la_SOURCES = decode_i386.c dct64_i386.c decode_i586.s
libmp586mmx_la_SOURCES = decode_i386.c dct64_MMX.s decode_MMX.s tabinit_MMX.s
libmp686_la_SOURCES = decode_i386.c dct64_i386.c decode_i586.s
libmp686mmx_la_SOURCES = decode_i386.c dct64_MMX.s decode_MMX.s tabinit_MMX.s
libmpppc_la_SOURCES = decode.c dct64.c
libmpplain_la_SOURCES = decode.c dct64.c

AM_CFLAGS = $(COMMON_CFLAGS) $(ARCH_CFLAGS)

lib_LTLIBRARIES = libmpg123.la
libmpg123_la_LDFLAGS = -module -avoid-version
libmpg123_la_LIBADD = ./$(MPG123_PLAT_LIB)
libmpg123_la_DEPENDENCIES = ./$(MPG123_PLAT_LIB)


