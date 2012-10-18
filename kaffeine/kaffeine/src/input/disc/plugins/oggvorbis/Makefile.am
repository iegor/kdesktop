kde_module_LTLIBRARIES = libkaffeineoggvorbis.la

INCLUDES = -I$(top_srcdir)/kaffeine/src/ -I$(top_srcdir)/kaffeine/src/input/disc/plugins/ $(all_includes) $(CFLAGS_OGGVORBIS)

METASOURCES = AUTO

kaffeineincludedir = $(includedir)/kaffeine

noinst_HEADERS =  koggenc.h

libkaffeineoggvorbis_la_SOURCES = koggenc.cpp oggconfig.ui
libkaffeineoggvorbis_la_LIBADD  = ../libkaffeineaudioencoder.la $(LIB_OGGVORBIS)
libkaffeineoggvorbis_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries) -avoid-version -no-undefined

# this is where the desktop file will go
partdesktopdir   = $(kde_servicesdir)
partdesktop_DATA = kaffeineoggvorbis.desktop
