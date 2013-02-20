INCLUDES= $(all_includes)

kde_module_LTLIBRARIES = noatunalsaplayer.la

noatunalsaplayer_la_SOURCES = \
  AlsaPlayer.ui \
  alsaplayerui.cpp \
  userinterface.cpp \
  configmodule.cpp \
  seeker.cpp

noatunalsaplayer_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module

noatunalsaplayer_la_LIBADD = -lnoatuncontrols -lnoatun

noatunalsaplayer_la_METASOURCES = AUTO

noinst_HEADERS = userinterface.h configmodule.h seeker.h

noatun_DATA = alsaplayerui.plugin
noatundir = $(kde_datadir)/noatun

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/alsaplayerui.pot
