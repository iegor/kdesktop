noinst_LTLIBRARIES = libucsi.la

SUBDIRS = dvb mpeg atsc .

INCLUDES = -I$(top_srcdir)/kaffeine/src/input/dvb/lib

libucsi_la_SOURCES = crc32.c \
           section_buf.c \
           transport_packet.c

libucsi_la_LDFLAGS = ./mpeg \
	./dvb \
	/atsc

libucsi_la_LIBADD   = ./mpeg/libdvbmpeg.la \
	./dvb/libdvbdvb.la \
	./atsc/libdvbatsc.la

