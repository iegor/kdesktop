
SUBDIRS= . pics

INCLUDES = \
	-I$(top_srcdir)/arts/gui/kde \
	-I$(top_builddir)/arts/gui/common \
	-I$(top_builddir)/arts/modules \
	-I$(top_builddir)/arts/modules/common \
	-I$(top_builddir)/arts/modules/synth \
	-I$(top_builddir)/arts/modules/mixers \
	-I$(top_builddir)/arts/modules/effects \
	-I$(top_builddir)/arts/midi \
	-I$(kde_includes)/arts \
	$(all_includes)


lib_LTLIBRARIES = libartscontrolsupport.la libartscontrolapplet.la

libartscontrolapplet_la_SOURCES = \
	artscontrolapplet.cpp

libartscontrolsupport_la_SOURCES = \
	templateview.cpp artsactions.cpp \
	audiomanager.cpp choosebusdlg.cpp \
	midimanagerview.cpp midimanagerdlg.ui midimanagerwidget.ui \
	midiportdlg.cpp midiinstdlg.cpp environmentview.cpp \
	fftscopeview.cpp mediatypesview.cpp statusview.cpp

METASOURCES = AUTO

SOMAJOR = 1
SOMINOR = 0
SOSUBMINOR = 0

libartscontrolapplet_la_LDFLAGS = $(all_libraries) -version-info $(SOMAJOR):$(SOMINOR):$(SOSUBMINOR) -no-undefined -module
libartscontrolsupport_la_LDFLAGS = $(all_libraries) -version-info $(SOMAJOR):$(SOMINOR):$(SOSUBMINOR) -no-undefined

libartscontrolapplet_la_LIBADD = \
	-lartsflow -lartsflow_idl -lmcop -lqtmcop $(LIBDL) -lsoundserver_idl -lartskde \
	$(LIB_KDEUI) ./libartscontrolsupport.la

# Arnolds version with dynamic-linking for testing.
#
#libartscontrolsupport_la_LIBADD = \
#	-lartsflow -lartsflow_idl -lmcop -lqtmcop $(LIBDL) -lsoundserver_idl -lartskde \
#	$(top_builddir)/arts/gui/common/libartsgui_idl.la \
#	-L$(top_builddir)/arts/gui/kde -lartsgui_kde \
#	-L$(top_builddir)/arts/modules -lartsmodules \
#	$(LIB_KDEUI)

libartscontrolsupport_la_LIBADD = \
	-lartsflow -lartsflow_idl -lmcop -lqtmcop $(LIBDL) -lsoundserver_idl -lartskde \
	$(top_builddir)/arts/gui/common/libartsgui_idl.la \
	$(top_builddir)/arts/gui/kde/libartsgui_kde.la \
	$(top_builddir)/arts/modules/libartsmodules.la \
	$(LIB_KDEUI)

bin_PROGRAMS = artscontrol
artscontrol_SOURCES = main.cpp levelmeters.cpp
artscontrol_LDFLAGS = $(all_libraries) $(KDE_RPATH)
artscontrol_LDADD = libartscontrolsupport.la \
	$(LIB_KDEUI) $(LIB_KSYCOCA) $(LIB_KFILE) \
	-lqtmcop -lartsflow_idl -lsoundserver_idl -lartskde

xdg_apps_DATA = artscontrol.desktop

lnkdir = $(kde_datadir)/kicker/applets
lnk_DATA = artscontrolapplet.desktop

rcdir = $(kde_datadir)/artscontrol
rc_DATA = artscontrol.rc artsmidimanagerview.rc

messages: rc.cpp
	$(XGETTEXT) *.h *.cpp -o $(podir)/artscontrol.pot

artsactions.lo: ../modules/artsmodules.h ../midi/artsmidi.h ../gui/common/artsgui.h ../modules/common/artsmodulescommon.h ../modules/synth/artsmodulessynth.h ../modules/effects/artsmoduleseffects.h ../modules/mixers/artsmodulesmixers.h
artscontrolapplet.lo: ../gui/common/artsgui.h ../modules/effects/artsmoduleseffects.h ../modules/common/artsmodulescommon.h ../midi/artsmidi.h ../modules/synth/artsmodulessynth.h ../modules/artsmodules.h ../modules/mixers/artsmodulesmixers.h
environmentview.lo: ../modules/artsmodules.h ../midi/artsmidi.h ../gui/common/artsgui.h ../modules/common/artsmodulescommon.h ../modules/synth/artsmodulessynth.h ../modules/effects/artsmoduleseffects.h ../modules/mixers/artsmodulesmixers.h
fftscopeview.lo: ../modules/artsmodules.h ../midi/artsmidi.h ../gui/common/artsgui.h ../modules/common/artsmodulescommon.h ../modules/synth/artsmodulessynth.h ../modules/effects/artsmoduleseffects.h ../modules/mixers/artsmodulesmixers.h
main.o: ../gui/common/artsgui.h ../modules/artsmodules.h ../midi/artsmidi.h ../modules/common/artsmodulescommon.h ../modules/synth/artsmodulessynth.h ../modules/effects/artsmoduleseffects.h ../modules/mixers/artsmodulesmixers.h
midimanagerview.lo: ../midi/artsmidi.h ../modules/artsmodules.h ../gui/common/artsgui.h ../modules/common/artsmodulescommon.h ../modules/synth/artsmodulessynth.h ../modules/effects/artsmoduleseffects.h ../modules/mixers/artsmodulesmixers.h

