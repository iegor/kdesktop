# set the include path for X, qt and KDE
#INCLUDES= $(all_includes)
AM_CPPFLAGS = $(all_includes)

# We use deprecated routines in QButton, undefine QT_NO_COMPAT.
KDE_CXXFLAGS = -UQT_NO_COMPAT

METASOURCES = AUTO
#bin_PROGRAMS = knetworkconf

kde_module_LTLIBRARIES = kcm_knetworkconfmodule.la

kcm_knetworkconfmodule_la_LIBADD   =  $(LIB_KDEUI)

# the library search path. 
kcm_knetworkconfmodule_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined

## INCLUDES were found outside kdevelop specific part

kcm_knetworkconfmodule_la_SOURCES = kselectdistrodlg.ui kknownhostinfo.cpp \
		kinterfaceupdowndlg.ui kaddknownhostdlg.ui kdetectdistrodlg.ui knetworkconfmodule.cpp \
		kroutinginfo.cpp kreloadnetworkdlg.ui kaddressvalidator.cpp kdnsinfo.cpp \
		knetworkinfo.cpp kadddevicedlg.ui kadddnsserverdlg.ui knetworkconfigparser.cpp \
		knetworkinterface.cpp knetworkconfdlg.ui knetworkconf.cpp main.cpp kadddevicedlgextension.ui \
		knetworkconfiface.skel kwirelessinterface.cpp kadddevicewifiext.ui 	kadddevicecontainer.cpp \
	kprofileslistviewtooltip.cpp


EXTRA_DIST = main.cpp knetworkconf.cpp knetworkconf.h knetworkconf.desktop cr16-app-knetworkconf.png cr22-app-knetworkconf.png cr32-app-knetworkconf.png knetworkconfdlg.ui knetworkinterface.cpp knetworkinterface.h knetworkconfigparser.cpp knetworkconfigparser.h kadddnsserverdlg.ui kadddevicedlg.ui knetworkinfo.cpp knetworkinfo.h kdnsinfo.cpp kdnsinfo.h kaddressvalidator.cpp kaddressvalidator.h kreloadnetworkdlg.ui network_card.png kroutinginfo.cpp kroutinginfo.h knetworkconfmodule.cpp knetworkconfmodule.h kdetectdistrodlg.ui kaddknownhostdlg.ui kinterfaceupdowndlg.ui kadddnsserverdlg.ui.h kadddevicedlg.ui.h kaddknownhostdlg.ui.h kknownhostinfo.cpp kknownhostinfo.h kselectdistrodlg.ui kcm_knetworkconfmodule version.h

xdg_apps_DATA = kcm_knetworkconfmodule.desktop 

KDE_ICON = AUTO

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/knetworkconf.pot; \
	fi

noinst_HEADERS = kprofileslistviewtooltip.h
