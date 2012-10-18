SUBDIRS = player-parts input

bin_PROGRAMS = kaffeine

INCLUDES = -I$(top_srcdir)/kaffeine/src/input/ \
	-I$(top_srcdir)/kaffeine/src/input/dvb/ \
	-I$(top_builddir)/kaffeine/src/input/dvb/plugins/epg \
	-I$(top_srcdir)/kaffeine/src/input/dvb/plugins/epg \
	-I$(top_builddir)/kaffeine/src/input/dvb/ \
	-I$(top_builddir)/kaffeine/src/input/disc/ \
	-I$(top_srcdir)/kaffeine/src/input/disc/ \
	-I$(top_srcdir)/kaffeine/src/input/disc/plugins \
	-I$(top_srcdir)/kaffeine/src/player-parts/kaffeine-part \
	-I$(top_srcdir)/kaffeine/src/input/dvbclient/ \
	-I$(top_srcdir)/kaffeine/src/input/audiobrowser/ \
	$(all_includes) $(CFLAGS_DPMS) $(CFLAGS_XTEST)

METASOURCES = AUTO

kaffeine_SOURCES = main.cpp \
	kaffeine.cpp \
	startwindow.cpp \
	pref.cpp \
	kaffeineiface.skel \
	systemtray.cpp \
	instwizard.cpp\
	kmultitabbar.cpp \
	inputmanager.cpp

kaffeine_LDFLAGS = $(all_libraries) $(KDE_RPATH)

if with_dvb
    DVBLIB = input/dvb/libkaffeinedvb.la
endif

kaffeine_LDADD   = $(LIB_KPARTS) \
	player-parts/kaffeine-part/libkaffeinepart.la \
	input/dvbclient/libkaffeinedvbclient.la \
	input/audiobrowser/libkaffeineaudiobrowser.la \
	input/disc/libkaffeinedisc.la \
	$(DVBLIB) $(LIB_DPMS) $(LIB_XTEST)

# this is where the desktop file will go
shelldesktopdir   = $(xdg_appsdir)
shelldesktop_DATA = kaffeine.desktop

# this is where the XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/kaffeine
shellrc_DATA = kaffeineui.rc

# this is where profile file for KDE-LIRC goes
profiledata_DATA = kaffeine.profile.xml
profiledatadir = $(kde_datadir)/profiles
