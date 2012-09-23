AUTOMAKE_OPTIONS = foreign

KDE_CXXFLAGS = -DQT_PLUGIN

INCLUDES = $(all_includes)

noinst_HEADERS = kwinqinxconfig.h

kde_module_LTLIBRARIES = kwin_qinx_config.la
kwin_qinx_config_la_SOURCES = kwinqinxconfig.cc configdialog.ui
kwin_qinx_config_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kwin_qinx_config_la_LIBADD = $(LIB_KDEUI)
kwin_qinx_config_la_METASOURCES = AUTO

DISTCLEANFILES = $(kwin_qinx_config_la_METASOURCES)

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kwin_qinx_config.pot
