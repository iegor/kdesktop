INCLUDES = -I$(top_srcdir)/kview $(all_includes)

kde_module_LTLIBRARIES = kview_browserplugin.la

kview_browserplugin_la_SOURCES = kmyfileitemlist.cpp kviewbrowser.cpp
kview_browserplugin_la_LIBADD = $(LIB_KFILE) $(LIB_KPARTS) -lkdeprint
kview_browserplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

plugdir   = $(kde_datadir)/kviewviewer/kpartplugins
plug_DATA = kviewbrowser.desktop kviewbrowser.rc

METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kviewbrowserplugin.pot
