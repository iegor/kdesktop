kde_module_LTLIBRARIES = libxinepart.la

INCLUDES = -I$(top_srcdir)/kaffeine/src/ -I$(top_srcdir)/kaffeine/src/player-parts/kaffeine-part $(all_includes) $(CFLAGS_XINERAMA) $(CFLAGS_XINE) $(CFLAGS_XCB)

METASOURCES = AUTO

kaffeineincludedir = $(includedir)/kaffeine
kaffeineinclude_HEADERS = xine_part.h xine_part_iface.h

noinst_HEADERS =  xine_part_iface.h kxinewidget.h postfilter.h deinterlacequality.h videosettings.h \
		  filterdialog.h screenshotpreview.h xineconfig.h positionslider.h equalizer.h

libxinepart_la_SOURCES = xine_part.cpp kxinewidget.cpp postfilter.cpp deinterlacequality.cpp \
                             videosettings.cpp filterdialog.cpp screenshotpreview.cpp xineconfig.cpp positionslider.cpp \
                             equalizer.cpp xine_part_iface.skel
libxinepart_la_LIBADD  = $(LIB_XINERAMA) $(LIB_XINE) $(LIB_XCB) ../kaffeine-part/libkaffeinepart.la
libxinepart_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries) -avoid-version -no-undefined


# this is where the desktop file will go
partdesktopdir   = $(kde_servicesdir)
partdesktop_DATA = xine_part.desktop

# this is where the part's XML-GUI resource file goes
partrcdir   = $(kde_datadir)/kaffeine
partrc_DATA = xine_part.rc
