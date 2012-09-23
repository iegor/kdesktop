
if compile_ogg_export
OGGEXPORTDIR=ogg_export
endif

if compile_mp3_export
MP3EXPORTDIR=mp3_export
endif

SUBDIRS = . pics $(OGGEXPORTDIR) $(MP3EXPORTDIR)

INCLUDES= \
	-I$(top_builddir)/arts/gui/common \
	-I$(top_builddir)/arts/midi \
	-I$(top_builddir)/arts/modules/common \
	-I$(top_builddir)/arts/modules/synth \
	-I$(top_builddir)/arts/modules/effects \
	-I$(top_srcdir)/arts/gui/kde \
	-I$(top_srcdir)/arts/tools \
        -I$(top_builddir)/arts/modules/effects \
        -I$(top_builddir)/arts/modules/common \
        -I$(top_builddir)/arts/modules/synth \
        -I$(top_builddir)/arts/midi \
	-I$(arts_includes) $(all_includes)

METASOURCES = AUTO

bin_PROGRAMS =
lib_LTLIBRARIES =
kdeinit_LTLIBRARIES = krec.la

kde_module_LTLIBRARIES = kcm_krec.la kcm_krec_files.la libkrecexport_wave.la

noinst_LTLIBRARIES = lib_krec_common.la

krec_la_SOURCES = \
		krecnewproperties.cpp \
		krecfile.cpp \
		krecfileview.cpp \
		krecfileviewhelpers.cpp \
		krecfilewidgets.cpp \
		main.cpp \
		krecord.cpp

krec_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
krec_la_LIBADD = \
	lib_krec_common.la \
	$(top_builddir)/arts/gui/kde/libartsgui_kde.la \
	$(top_builddir)/arts/tools/libartscontrolsupport.la \
	-lartskde $(LIBDL) $(LIB_KIO) $(LIB_KDEUI) $(LIB_KUTILS)

kcm_krec_la_SOURCES = krecconfigure.cpp
kcm_krec_la_LDFLAGS = -module -avoid-version -no-undefined $(all_libraries)
kcm_krec_la_LIBADD = lib_krec_common.la $(LIB_KUTILS)

kcm_krec_files_la_SOURCES = krecconfig_files.cpp
kcm_krec_files_la_LDFLAGS = -module -avoid-version -no-undefined $(all_libraries)
kcm_krec_files_la_LIBADD = lib_krec_common.la $(LIB_KUTILS)

lib_krec_common_la_SOURCES = \
	krecglobal.cpp \
	krecexport_template.cpp \
	krecconfig_fileswidget.cpp

lib_krec_common_la_LDFLAGS = -module -avoid-version -no-undefined $(all_libraries)
lib_krec_common_la_LIBADD = $(LIB_KDEUI) $(LIB_KIO)

libkrecexport_wave_la_SOURCES = krecexport_wave.cpp
libkrecexport_wave_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
libkrecexport_wave_la_LIBADD = lib_krec_common.la

rcdir = $(kde_datadir)/krec
rc_DATA = krecui.rc

xdg_apps_DATA = krec.desktop
xdg_configdir = $(kde_servicesdir)
xdg_config_DATA = kcm_krec.desktop kcm_krec_files.desktop

messages: rc.cpp
	rm -f tips.cpp
	$(PREPARETIPS) > tips.cpp
	$(XGETTEXT) */*.cpp *.cpp *.h -o $(podir)/krec.pot
	rm -f tips.cpp

KDE_ICON = krec

appsdatadir=$(kde_datadir)/krec
appsdata_DATA=tips

kde_servicetypes_DATA=krec_exportitem.desktop

kde_services_DATA=krec_exportwave.desktop

krecord.lo: ../arts/gui/common/artsgui.h ../arts/modules/effects/artsmoduleseffects.h ../arts/modules/common/artsmodulescommon.h ../arts/midi/artsmidi.h ../arts/modules/synth/artsmodulessynth.h

