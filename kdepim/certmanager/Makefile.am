SUBDIRS = lib conf kwatchgnupg .

KDE_CXXFLAGS = -DLIBKLEOPATRA_NO_COMPAT

INCLUDES = -I$(srcdir)/lib -I$(top_srcdir)/libkdenetwork $(GPGME_CFLAGS) $(all_includes)

bin_PROGRAMS = kleopatra

kleopatra_SOURCES = \
	customactions.cpp \
	aboutdata.cpp \
	main.cpp certmanager.cpp \
	hierarchyanalyser.cpp \
	certificatewizard.ui certificatewizardimpl.cpp \
	certificateinfowidget.ui certificateinfowidgetimpl.cpp \
	crlview.cpp storedtransferjob.cpp certlistview.cpp

METASOURCES = AUTO

kleopatra_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kleopatra_LDADD   = conf/libconf.la lib/libkleopatra.la $(LIB_KUTILS) $(LIB_KABC)

rcdir = $(kde_datadir)/kleopatra
rc_DATA = kleopatraui.rc

xdg_apps_DATA = kleopatra_import.desktop

messages: rc.cpp
	$(EXTRACTRC) conf/*.ui >> rc.cpp
	$(XGETTEXT) conf/*.cpp *.cpp *.h -o $(podir)/kleopatra.pot

include $(top_srcdir)/admin/Doxyfile.am
