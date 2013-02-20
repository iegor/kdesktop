INCLUDES= $(all_includes)
SUBDIRS = pics toolbar
KDE_ICON=AUTO
xdg_apps_DATA = ksysv.desktop

bin_PROGRAMS = 	ksysv
# Which sources should be compiled for ksysv.
ksysv_SOURCES	=	main.cpp \
					ActionList.cpp \
					IOCore.cpp \
					ksvdraglist.cpp \
					RunlevelAuthIcon.cpp \
					OldView.cpp \
					Properties.cpp \
					SpinBox.cpp \
					ServiceDlg.cpp \
					TopWidget.cpp \
					Data.cpp \
					ksv_conf.cpp \
					ksvapplication.cpp \
					kdltooltip.cpp \
					ksv_core.cpp \
					ksvdrag.cpp \
					kscroller.cpp \
					ksv_service.cpp \
					leveldb.c \
					PreferencesDialog.cpp \
					trash.cpp \
					pathconfig.ui \
					ksvpathconfig.cpp \
					miscconfig.ui \
					ksvmiscconfig.cpp \
					lookandfeelconfig.ui \
					ksvlookandfeel.cpp \
					configwizard.ui \
					ksvconfigwizard.cpp \
					kbusymanager.cpp

# the library search path
ksysv_METASOURCES = AUTO
ksysv_LDFLAGS = $(all_libraries) $(KDE_RPATH)
ksysv_LDADD	=   $(LIB_KFILE)  -lkdeprint

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = ActionList.h \
					IOCore.h \
					OldView.h \
					Properties.h \
					SpinBox.h \
					Data.h \
					ksvapplication.h \
					ServiceDlg.h \
					TopWidget.h \
					kdltooltip.h \
					ksvdrag.h \
					ksv_service.h \
					ksv_conf.h \
					RunlevelAuthIcon.h \
					ksv_core.h \
					kscroller.h \
					ksvdraglist.h \
					leveldb.h \
					PreferencesDialog.h \
					trash.h \
					ksvpathconfig.h \
					ksvmiscconfig.h \
					ksvlookandfeel.h \
					ksvconfigwizard.h \
					kbusymanager.h \
					version.h

# just to make sure, automake makes them 

messages: rc.cpp
	$(XGETTEXT) -C *.cpp -o $(podir)/ksysv.pot

rcdir = $(kde_datadir)/ksysv
rc_DATA = ksysvui.rc

kdemime_DATA = x-ksysv.desktop
kdemimedir = $(kde_mimedir)/application

textmime_DATA = x-ksysv-log.desktop
textmimedir= $(kde_mimedir)/text
