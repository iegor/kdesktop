INCLUDES = -I$(top_srcdir)/kview $(all_includes)

kde_module_LTLIBRARIES = kview_templateplugin.la

kview_templateplugin_la_SOURCES = kviewtemplate.cpp
kview_templateplugin_la_LIBADD = $(LIB_KFILE) $(LIB_KPARTS) -lkdeprint
kview_templateplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

plugdir   = $(kde_datadir)/kview/kpartplugins
plug_DATA = kviewtemplate.desktop kviewtemplate.rc

METASOURCES = AUTO

messages: rc.cpp
	    $(XGETTEXT) *.cpp *.h -o $(podir)/kviewtemplateplugin.pot
