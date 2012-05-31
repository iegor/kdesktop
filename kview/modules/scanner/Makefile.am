INCLUDES = -I$(top_srcdir)/kview $(all_includes)

kde_module_LTLIBRARIES = kview_scannerplugin.la

kview_scannerplugin_la_SOURCES = kviewscanner.cpp
kview_scannerplugin_la_LIBADD = $(LIB_KFILE) $(LIB_KPARTS) -lkdeprint
kview_scannerplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

plugdir   = $(kde_datadir)/kview/kpartplugins
plug_DATA = kviewscanner.desktop kviewscanner.rc

METASOURCES = AUTO

messages: rc.cpp
	    $(XGETTEXT) *.cpp *.h -o $(podir)/kviewscannerplugin.pot
