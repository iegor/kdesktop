kde_module_LTLIBRARIES = kcm_kviewcanvasconfig.la
INCLUDES = $(all_includes)

noinst_HEADERS = confmodules.h

kcm_kviewcanvasconfig_la_SOURCES = generalconfigwidget.ui confmodules.cpp
kcm_kviewcanvasconfig_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) -module -avoid-version
kcm_kviewcanvasconfig_la_LIBADD  = $(LIB_KDEUI)

kcm_kviewcanvasconfig_DATA = kviewcanvasconfig.desktop
kcm_kviewcanvasconfigdir = $(kde_servicesdir)/kconfiguredialog

METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kcm_kviewcanvasconfig.pot
