INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = libkmidpart.la
lib_LTLIBRARIES = libkmidlib.la

libkmidlib_la_LDFLAGS = $(all_libraries)
libkmidlib_la_LIBADD = $(LIB_KFILE) $(LIB_KPARTS) $(LIB_KIO) -lkmid

libkmidpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
libkmidpart_la_LIBADD = libkmidlib.la

libkmidlib_la_SOURCES = kmidframe.cpp kmidclient.cpp kdisptext.cpp \
	qslidertime.cpp klcdnumber.cpp ktrianglebutton.cpp \
	midicfgdlg.cpp collectdlg.cpp randomlist.cpp \
	channelview.cpp channel.cpp channel3d.cpp channel4d.cpp \
	channelcfgdlg.cpp instrname.cpp rhythmview.cpp \
	songlist.cpp slman.cpp kmid_part.cpp kmidIface.skel

libkmidpart_la_SOURCES = part_dummy.cpp

part_dummy.cpp:
	touch part_dummy.cpp

noinst_HEADERS = kmidframe.h kmidclient.h kdisptext.h \
        qslidertime.h klcdnumber.h ktrianglebutton.h \
        midicfgdlg.h collectdlg.h randomlist.h \
        channelview.h channel.h channel3d.h channel4d.h \
        channelcfgdlg.h instrname.h rhythmview.h \
        songlist.h slman.h kmid_part.h

kmid_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kmid_SOURCES = main.cpp
kmid_LDADD   = libkmidlib.la

bin_PROGRAMS = kmid
METASOURCES = AUTO

servicetype_DATA = audiomidi.desktop
servicetypedir = $(kde_servicetypesdir)

SUBDIRS = maps examples pics

xdg_apps_DATA = kmid.desktop

mime_DATA = x-karaoke.desktop
mimedir = $(kde_mimedir)/audio

rc_DATA = kmidui.rc
rcdir = $(kde_datadir)/kmid

KDE_ICON = kmid

fmpatches_DATA = std.sb drums.sb std.o3 drums.o3
fmpatchesdir = $(kde_datadir)/kmid/fm

partdir   = $(kde_datadir)/kmid
part_DATA = kmid_partui.rc

channelview_DATA = keyboard.xpm button1.xpm button2.xpm
channelviewdir = $(kde_datadir)/kmid/icons

EXTRA_DIST = $(xdg_apps_DATA) $(mime_DATA) $(fmpatches_DATA) $(part_DATA)

instrname.i18n: instrname.cpp
	echo "/* This is an automatically generated file" > instrname.i18n
	echo " * DO NOT EDIT !!!" >> instrname.i18n
	echo " * To create this file run : ""make instrname.i18n"" " >> instrname.i18n
	echo " */" >> instrname.i18n
	echo "" >> instrname.i18n
	awk -f instrfilter.awk instrname.cpp >> instrname.i18n


messages: instrname.i18n rc.cpp
	$(XGETTEXT) *.cpp instrname.i18n -o $(podir)/kmid.pot
