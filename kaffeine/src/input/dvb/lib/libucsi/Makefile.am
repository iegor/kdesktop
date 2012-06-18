noinst_LTLIBRARIES = libucsi.la

SUBDIRS = dvb mpeg .

INCLUDES = -I$(top_srcdir)/kaffeine/src/input/dvb/lib

libucsi_la_SOURCES = crc32.c \
           section_buf.c \
           transport_packet.c

libucsi_la_LDFLAGS = ./mpeg \
	./dvb

libucsi_la_LIBADD   = ./mpeg/libdvbmpeg.la \
	./dvb/libdvbdvb.la

CFLAGS  = -g -O2 -Wall -Wshadow -Wpointer-arith -Wstrict-prototypes -fPIC
