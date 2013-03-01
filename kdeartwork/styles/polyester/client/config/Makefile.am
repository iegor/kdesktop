AUTOMAKE_OPTIONS = foreign

KDE_CXXFLAGS = -DQT_PLUGIN

METASOURCES = AUTO

INCLUDES = $(all_includes)

noinst_HEADERS = polyesterconfig.h

kde_module_LTLIBRARIES = kwin_polyester_config.la
kwin_polyester_config_la_SOURCES = configdialog.ui polyesterconfig.cc
kwin_polyester_config_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kwin_polyester_config_la_LIBADD = $(LIB_KDEUI)
