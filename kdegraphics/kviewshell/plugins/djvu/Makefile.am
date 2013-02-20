INCLUDES = -I$(top_srcdir) $(all_includes) \
	-I$(top_srcdir)/kviewshell \
	-I$(top_builddir)/kviewshell \
	-I$(kde_includes)/kviewshell \
	-I$(srcdir)/libdjvu

SUBDIRS = libdjvu .

KDE_CXXFLAGS = -Wno-deprecated

METASOURCES = AUTO

# this is where the desktop file will go
kde_services_DATA = djvumultipage.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/kviewshell/plugins/djvu

kde_module_LTLIBRARIES = djvuviewpart.la
djvuviewpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
djvuviewpart_la_LIBADD = -lkdeprint -lkparts $(top_builddir)/kviewshell/libkmultipage.la libdjvu/libdjvu.la
djvuviewpart_la_SOURCES = djvumultipage.cpp djvurenderer.cpp kprintDialogPage_DJVUpageoptions.cpp \
	kprintDialogPage_DJVUconversionoptions.cpp kprintDialogPage_DJVUconversionoptions_basewidget.ui \
	pageRangeWidget_base.ui pageRangeWidget.cpp \
	prefs.kcfgc

kde_kcfg_DATA = djvumultipage.kcfg

pluginsdir = $(kde_datadir)
plugins_DATA = djvumultipage.rc

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kdjview.pot
