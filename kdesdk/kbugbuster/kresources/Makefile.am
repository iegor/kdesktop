INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/kbugbuster/backend $(all_includes)

kde_module_LTLIBRARIES = kcal_bugzilla.la

kcal_bugzilla_la_SOURCES = kcalresource.cpp kcalresourceconfig.cpp \
                           kcalresource_plugin.cpp resourceprefs.kcfgc
kcal_bugzilla_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kcal_bugzilla_la_LIBADD  = ../backend/libkbbbackend.la -lkcal

servicedir = $(kde_servicesdir)/kresources/kcal
service_DATA = bugzilla.desktop

METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kres_bugzilla.pot
