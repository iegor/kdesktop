SUBDIRS = .

INCLUDES = -I$(top_srcdir)/kview $(all_includes)

kde_module_LTLIBRARIES = kview_presenterplugin.la

kview_presenterplugin_la_SOURCES = imagelistitem.cpp imagelistdialog.ui kviewpresenter.cpp
kview_presenterplugin_la_LIBADD = $(LIB_KIO) $(LIB_KPARTS)
kview_presenterplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

plugdir   = $(kde_datadir)/kview/kpartplugins
plug_DATA = kviewpresenter.desktop kviewpresenter.rc

METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kviewpresenterplugin.pot
