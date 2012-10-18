
kde_module_LTLIBRARIES = libdummypart.la

INCLUDES = -I$(top_srcdir)/kaffeine/src/player-parts/ $(all_includes)

METASOURCES = AUTO

noinst_HEADERS =  dummy_part.h

libdummypart_la_SOURCES = dummy_part.cpp
libdummypart_la_LIBADD  = $(LIB_KPARTS) $(LIB_KFILE) ../libkaffeinepart.la
libdummypart_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries) -avoid-version -no-undefined

# this is where the desktop file will go
partdesktopdir   = $(kde_servicesdir)
partdesktop_DATA = dummy_part.desktop

# this is where the part's XML-GUI resource file goes
partrcdir   = $(kde_datadir)/dummypart
partrc_DATA = dummy_part.rc