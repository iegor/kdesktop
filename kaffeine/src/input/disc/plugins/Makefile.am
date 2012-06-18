if with_lame
    LAME_SUBDIR = mp3lame
endif

if with_oggvorbis
    OGGVORBIS_SUBDIR = oggvorbis
endif

lib_LTLIBRARIES = libkaffeineaudioencoder.la

METASOURCES = AUTO

SUBDIRS = . $(LAME_SUBDIR) $(OGGVORBIS_SUBDIR)

INCLUDES = -I$(top_srcdir)/kaffeine/src $(all_includes)

kaffeineincludedir = $(includedir)/kaffeine
kaffeineinclude_HEADERS = kaffeineaudioencoder.h

libkaffeineaudioencoder_la_SOURCES = kaffeineaudioencoder.cpp
libkaffeineaudioencoder_la_LIBADD  = $(LIB_KPARTS)
libkaffeineaudioencoder_la_LDFLAGS = $(all_libraries) -version-info 0:1:0 -no-undefined

kde_servicetypes_DATA = kaffeineaudioencoder.desktop
