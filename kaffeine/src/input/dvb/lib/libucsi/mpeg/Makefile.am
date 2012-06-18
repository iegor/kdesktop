noinst_LTLIBRARIES = libdvbmpeg.la

INCLUDES = -I$(top_srcdir)/kaffeine/src/input/dvb/lib

libdvbmpeg_la_SOURCES = cat_section.c \
           metadata_section.c \
           odsmt_section.c \
           pat_section.c \
           pmt_section.c \
           tsdt_section.c

CFLAGS  = -g -O2 -Wall -Wshadow -Wpointer-arith -Wstrict-prototypes -fPIC
