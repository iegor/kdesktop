INCLUDES = -I$(top_srcdir)/kview $(all_includes)

kde_module_LTLIBRARIES = kview_effectsplugin.la

kview_effectsplugin_la_SOURCES = kvieweffects.cpp
kview_effectsplugin_la_LIBADD = $(LIB_KFILE) $(LIB_KPARTS) -lkdeprint
kview_effectsplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

plugdir   = $(kde_datadir)/kview/kpartplugins
plug_DATA = kvieweffects.desktop kvieweffects.rc

METASOURCES = AUTO

messages: rc.cpp
	    $(XGETTEXT) *.cpp *.h -o $(podir)/kvieweffectsplugin.pot
