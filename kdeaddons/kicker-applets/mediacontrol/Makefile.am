SUBDIRS = . pics
INCLUDES =  $(XMMS_INCLUDES) $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = mediacontrol_panelapplet.la

mediacontrol_panelapplet_la_COMPILE_FIRST = mediacontrolconfigwidget.h
mediacontrol_panelapplet_la_SOURCES = mcslider.cpp \
	mediacontrol.cpp playerInterface.cpp \
	configfrontend.cpp mediacontrolconfigwidget.ui \
	mediacontrolconfig.cpp mediacontroliface.skel \
	noatunInterface.cpp \
	xmmsInterface.cpp \
	mpdInterface.cpp \
	jukInterface.cpp \
	amarokInterface.cpp \
	kscdInterface.cpp \
	simplebutton.cpp

mediacontrol_panelapplet_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries) $(XMMS_LIBS)
mediacontrol_panelapplet_la_LIBADD = $(LIB_KDEUI)

lnkdir   =  $(kde_datadir)/kicker/applets
lnk_DATA = mediacontrol.desktop
EXTRA_DIST = $(lnk_DATA)

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/mediacontrol.pot
