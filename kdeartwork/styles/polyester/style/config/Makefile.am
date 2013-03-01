INCLUDES = $(all_includes)

METASOURCES = AUTO

noinst_HEADERS = polyesterconf.h
kde_module_LTLIBRARIES = kstyle_polyester_config.la
kstyle_polyester_config_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kstyle_polyester_config_la_LIBADD  = $(LIB_KDEUI)
kstyle_polyester_config_la_SOURCES = polyesterconf.cpp configdialog.ui

messages:
	$(XGETTEXT) *.cpp *.h -o $(podir)/kstyle_polyester_config.pot
