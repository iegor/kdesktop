INCLUDES = -I$(top_srcdir)/kview $(all_includes)

kde_module_LTLIBRARIES = kview_scale.la

kview_scale_la_SOURCES = kfloatspinbox.cpp scaledlg.cpp kview_scale.cpp
kview_scale_la_LIBADD = $(LIB_KFILE) $(LIB_KPARTS) -lkdeprint
kview_scale_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

plugdir   = $(kde_datadir)/kviewviewer/kpartplugins
plug_DATA = kview_scale.rc kview_scale.desktop

METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kview_scale.pot
