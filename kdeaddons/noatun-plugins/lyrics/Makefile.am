INCLUDES= $(all_includes)
kde_module_LTLIBRARIES = noatunlyrics.la

noatunlyrics_la_SOURCES = lyrics.cpp historymanager.cpp cmodule.cpp

noatunlyrics_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatunlyrics_la_LIBADD = $(LIB_KHTML) $(LIB_KFILE) -lnoatun

noatunlyrics_la_METASOURCES = AUTO

noinst_HEADERS = lyrics.h

noatun_DATA = lyrics.plugin lyricsui.rc
noatundir = $(kde_datadir)/noatun

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/lyrics.pot
