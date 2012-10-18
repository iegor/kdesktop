SUBDIRS = lib plugins .

INCLUDES = -I$(top_srcdir)/kaffeine/src/input/ \
	-I$(top_srcdir)/kaffeine/src/input/dvb/lib \
	-I$(top_srcdir)/kaffeine/src/input/dvb/plugins/stream \
	-I$(top_srcdir)/kaffeine/src/input/dvb/plugins/epg \
	-I$(top_srcdir)/kaffeine/src \
	$(all_includes)

METASOURCES = AUTO

noinst_LTLIBRARIES = libkaffeinedvb.la

libkaffeinedvb_la_SOURCES = audioeditor.cpp \
	audioeditor.h \
	audioeditorui.ui \
	channeldesc.cpp \
	channeldesc.h \
	channeleditor.cpp \
	channeleditor.h \
	channeleditorui.ui \
	dvbconfig.cpp \
	dvbconfig.h \
	dvbevents.cpp \
	dvbevents.h \
	dvbout.cpp \
	dvbout.h \
	dvbpanel.cpp \
	dvbpanel.h \
	dvbsection.h \
	dvbsi.cpp \
	dvbsi.h \
	camdialog.ui \
	cammenudialog.ui \
	dvbstream.cpp \
	dvbstream.h \
	gdvb.h \
	kevents.cpp \
	kevents.h \
	kgradprogress.cpp \
	kgradprogress.h \
	krecord.cpp \
	krecord.h \
	ktimereditor.cpp \
	ktimereditor.h \
	scandialog.cpp \
	scandialog.h \
	scandialogui.ui \
	sender.cpp \
	sender.h \
	ts2rtp.cpp \
	ts2rtp.h \
	subeditorui.ui \
	subeditor.cpp \
	subeditor.h \
	broadcasteditorui.ui \
	broadcasteditor.cpp \
	broadcasteditor.h \
	cleaner.cpp \
	cleaner.h \
	crontimerui.ui \
	crontimer.cpp \
	crontimer.h \
	dvbcam.cpp \
	dvbcam.h

libkaffeinedvb_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) \
	-L$(top_srcdir)/kaffeine/src/input \
	-L$(top_srcdir)/kaffeine/src/input/dvb/plugins/stream \
	-L$(top_srcdir)/kaffeine/src/input/dvb/plugins/epg \
	./lib/libdvbapi \
	./lib/libdvben50221 \
	./lib/libucsi

libkaffeinedvb_la_LIBADD = \
	$(top_builddir)/kaffeine/src/input/dvb/plugins/stream/libkaffeinedvbplugin.la \
	$(top_builddir)/kaffeine/src/input/dvb/plugins/epg/libkaffeineepgplugin.la \
	$(top_builddir)/kaffeine/src/input/libkaffeineinput.la \
	./lib/libdvbapi/libdvbapi.la ./lib/libdvben50221/libdvben50221.la ./lib/libucsi/libucsi.la

# this is where the XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/kaffeine
shellrc_DATA = kaffeinedvb.rc
