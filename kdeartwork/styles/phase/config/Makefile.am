INCLUDES = $(all_includes)

noinst_HEADERS = phasestyleconfig.h

kde_module_LTLIBRARIES = kstyle_phase_config.la
kstyle_phase_config_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kstyle_phase_config_la_LIBADD  = $(LIB_KDEUI)
kstyle_phase_config_la_SOURCES = phasestyleconfig.cpp styledialog.ui
kstyle_phase_config_la_METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kstyle_phase_config.pot
