METASOURCES = AUTO

noinst_LTLIBRARIES = libkaffeineaudiobrowser.la

INCLUDES = -I$(top_srcdir)/kaffeine/src/input/ \
	-I$(top_srcdir)/kaffeine/src/ \
	$(all_includes) \
	-I$(top_srcdir)/kaffeine/src/player-parts/kaffeine-part

libkaffeineaudiobrowser_la_SOURCES = playlist.cpp \
	playlistitem.cpp \
	urllistview.cpp \
	googlefetcher.cpp \
	googlefetcherdialog.cpp

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

libkaffeineaudiobrowser_la_LDFLAGS = $(KDE_RPATH) \
	$(all_libraries) \
	-L$(top_srcdir)/kaffeine/src/input

libkaffeineaudiobrowser_la_LIBADD = $(top_builddir)/kaffeine/src/input/libkaffeineinput.la \
	$(top_builddir)/kaffeine/src/player-parts/kaffeine-part/libkaffeinepart.la \
	$(LIB_KHTML)

# this is where the XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/kaffeine
shellrc_DATA = kaffeineplaylist.rc
