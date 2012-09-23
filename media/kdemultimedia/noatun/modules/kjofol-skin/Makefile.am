INCLUDES	=	-I$(top_srcdir)/noatun/library \
				-I$(top_builddir)/noatun/library \
				-I$(top_builddir)/arts/modules \
				-I$(top_builddir)/arts/midi \
				-I$(top_builddir)/arts/modules/synth \
				-I$(top_builddir)/arts/modules/common \
				-I$(top_builddir)/arts/modules/effects \
				-I$(top_builddir)/arts/modules/mixers \
				-I$(top_builddir)/arts/gui/common \
				-I$(kde_includes)/kio \
				-I$(kde_includes)/arts \
				$(all_includes)

kde_module_LTLIBRARIES = noatun_kjofol.la
SUBDIRS= . skins
METASOURCES = AUTO

noatun_kjofol_la_SOURCES	=	kjloader.cpp kjwidget.cpp kjbutton.cpp kjseeker.cpp \
										kjsliders.cpp kjfont.cpp kjtextdisplay.cpp \
										kjvis.cpp kjequalizer.cpp kjbackground.cpp \
										noatunui.cpp parser.cpp \
										kjprefs.cpp \
										kjskinselectorwidget.ui kjguisettingswidget.ui

noatun_kjofol_la_LDFLAGS	=	$(all_libraries) -module -avoid-version -no-undefined
noatun_kjofol_la_LIBADD		=	$(LIB_KFILE) $(top_builddir)/noatun/library/libnoatun.la \
										$(LIB_KIO) -lm \
										$(top_builddir)/arts/modules/libartsmodules.la

noinst_HEADERS					=	kjloader.h kjwidget.h kjbutton.h kjseeker.h \
										kjsliders.h kjfont.h kjtextdisplay.h \
										kjvis.h kjequalizer.h kjbackground.h \
										parser.h kjprefs.h \
										kjskinselectorwidget.h kjguisettingswidget.h

noatun_modules_kjofol_DATA = kjofolui.plugin
noatun_modules_kjofoldir = $(kde_datadir)/noatun

kjloader.lo: ../../../arts/modules/artsmodules.h ../../../arts/midi/artsmidi.h ../../../arts/gui/common/artsgui.h ../../../arts/modules/common/artsmodulescommon.h ../../../arts/modules/synth/artsmodulessynth.h ../../../arts/modules/effects/artsmoduleseffects.h ../../../arts/modules/mixers/artsmodulesmixers.h ../../library/noatunarts/noatunarts.h
kjsliders.lo: ../../../arts/modules/artsmodules.h ../../../arts/midi/artsmidi.h ../../../arts/gui/common/artsgui.h ../../../arts/modules/common/artsmodulescommon.h ../../../arts/modules/synth/artsmodulessynth.h ../../../arts/modules/effects/artsmoduleseffects.h ../../../arts/modules/mixers/artsmodulesmixers.h
kjtextdisplay.lo: ../../../arts/modules/artsmodules.h ../../../arts/midi/artsmidi.h ../../../arts/gui/common/artsgui.h ../../../arts/modules/common/artsmodulescommon.h ../../../arts/modules/synth/artsmodulessynth.h ../../../arts/modules/effects/artsmoduleseffects.h ../../../arts/modules/mixers/artsmodulesmixers.h
