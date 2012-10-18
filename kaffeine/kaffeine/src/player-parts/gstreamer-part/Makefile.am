kde_module_LTLIBRARIES = libgstreamerpart.la

INCLUDES = -I$(top_srcdir)/kaffeine/src/player-parts/kaffeine-part -I$(top_srcdir)/kaffeine/src/ $(all_includes) $(CFLAGS_GSTREAMER)

METASOURCES = AUTO

noinst_HEADERS =  gstreamer_part.h timer.h video.h videosettings.h gstreamerconfig.h

libgstreamerpart_la_SOURCES = gstreamer_part.cpp video.cpp timer.cpp videosettings.cpp gstreamerconfig.cpp
libgstreamerpart_la_LIBADD  = $(LIB_KPARTS) $(LIB_KFILE) ../kaffeine-part/libkaffeinepart.la
libgstreamerpart_la_LDFLAGS = -module $(KDE_PLUGIN) $(LIB_GSTREAMER) -lgstinterfaces-0.10 $(all_libraries) -avoid-version -no-undefined

# this is where the desktop file will go
partdesktopdir   = $(kde_servicesdir)
partdesktop_DATA = gstreamer_part.desktop

# this is where the part's XML-GUI resource file goes
partrcdir   = $(kde_datadir)/gstreamerpart
partrc_DATA = gstreamer_part.rc

icondir = $(kde_icondir)/hicolor/16x16/apps/
icon_DATA = gstreamer.png
