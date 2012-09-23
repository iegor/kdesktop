INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = kcm_kviewpresenterconfig.la

noinst_HEADERS = kviewpresenterconfig.h

kcm_kviewpresenterconfig_la_SOURCES = kviewpresenterconfig.cpp
kcm_kviewpresenterconfig_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kcm_kviewpresenterconfig_la_LIBADD = $(LIB_KUTILS)

kcm_kviewpresenterconfig_DATA = kviewpresenterconfig.desktop
kcm_kviewpresenterconfigdir = $(kde_servicesdir)/kconfiguredialog

METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kcm_kviewpresenterconfig.pot
