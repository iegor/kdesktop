kde_module_LTLIBRARIES = libkaffeinemp3lame.la

INCLUDES = -I$(top_srcdir)/kaffeine/src/ -I$(top_srcdir)/kaffeine/src/input/disc/plugins/ $(all_includes)

METASOURCES = AUTO

kaffeineincludedir = $(includedir)/kaffeine

noinst_HEADERS =  klameenc.h

libkaffeinemp3lame_la_SOURCES = klameenc.cpp lameconfig.ui
libkaffeinemp3lame_la_LIBADD  = ../libkaffeineaudioencoder.la $(LIB_LAME)
libkaffeinemp3lame_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries) -avoid-version -no-undefined

# this is where the desktop file will go
partdesktopdir   = $(kde_servicesdir)
partdesktop_DATA = kaffeinemp3lame.desktop
