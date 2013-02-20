AUTOMAKE_OPTIONS = foreign

KDE_CXXFLAGS = -DQT_PLUGIN

INCLUDES = $(all_includes)

noinst_HEADERS = smoothblendconfig.h

kde_module_LTLIBRARIES = kwin_smoothblend_config.la
kwin_smoothblend_config_la_SOURCES = configdialog.ui smoothblendconfig.cc
kwin_smoothblend_config_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kwin_smoothblend_config_la_LIBADD = $(LIB_KDEUI)
kwin_smoothblend_config_la_METASOURCES = AUTO

DISTCLEANFILES = $(kwin_smoothblend_config_la_METASOURCES)
