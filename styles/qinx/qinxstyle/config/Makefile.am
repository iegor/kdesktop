AUTOMAKE_OPTIONS = foreign

INCLUDES = $(all_includes)

noinst_HEADERS = qinxstyleconfig.h

kde_module_LTLIBRARIES = kstyle_qinx_config.la
kstyle_qinx_config_la_SOURCES = qinxstyleconfig.cc styledialog.ui
kstyle_qinx_config_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kstyle_qinx_config_la_LIBADD  = $(LIB_KDEUI)
kstyle_qinx_config_la_METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kstyle_qinx_config.pot
