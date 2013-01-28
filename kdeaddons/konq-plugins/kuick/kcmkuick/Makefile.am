INCLUDES= $(all_includes) 

kde_module_LTLIBRARIES = kcm_kuick.la

kcm_kuick_la_SOURCES =  kcmkuick.cpp kcmkuickdialog.ui

kcm_kuick_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
kcm_kuick_la_LIBADD = $(LIB_KDEUI) $(LIB_KIO)
kcm_kuick_la_METASOURCES = AUTO

noinst_HEADERS = kcmkuick.h kcmkuickdialog.h

apps_DATA = kcmkuick.desktop
appsdir = $(kde_appsdir)/.hidden

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kcmkuick.pot
