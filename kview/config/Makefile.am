SUBDIRS = . plugins
INCLUDES = $(all_includes)

METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_kviewgeneralconfig.la

noinst_HEADERS = kviewconfmodules.h

kcm_kviewgeneralconfig_la_SOURCES = kviewconfmodules.cpp
kcm_kviewgeneralconfig_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kcm_kviewgeneralconfig_la_LIBADD = $(LIB_KDEUI)

kcm_kviewgeneralconfig_DATA = kviewgeneralconfig.desktop
kcm_kviewgeneralconfigdir = $(kde_servicesdir)/kconfiguredialog

kviewappdatadir = $(kde_datadir)/kview
kviewappdata_DATA = kview.setdlg

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kcm_kviewgeneralconfig.pot
