INCLUDES = $(all_includes)

METASOURCES = AUTO

kde_module_LTLIBRARIES = kcm_kviewpluginsconfig.la

noinst_HEADERS = kviewpluginsconfig.h

kcm_kviewpluginsconfig_la_SOURCES = kviewpluginsconfig.cpp
kcm_kviewpluginsconfig_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kcm_kviewpluginsconfig_la_LIBADD = $(LIB_KUTILS)

kcm_kviewpluginsconfig_DATA = kviewpluginsconfig.desktop
kcm_kviewpluginsconfigdir = $(kde_servicesdir)/kconfiguredialog

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kcm_kviewpluginsconfig.pot
