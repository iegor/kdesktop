SUBDIRS = . pics

AM_CPPFLAGS = $(all_includes)
METASOURCES = AUTO



################################
# these are the headers for your project
noinst_HEADERS = nspanel.h nsstacktabwidget.h sidebar_news.h norsswidget.h

kde_module_LTLIBRARIES = konq_sidebarnews.la

# the Part's source, library search path, and link libraries
konq_sidebarnews_la_SOURCES = sidebar_news.cpp sidebar_news.skel nsstacktabwidget.cpp \
                              nspanel.cpp nspanel.skel norsswidget.cpp \
                              sidebarsettings.kcfgc \
			      configfeedsbase.ui configfeeds.cpp
konq_sidebarnews_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
konq_sidebarnews_la_LIBADD  = $(LIB_KPARTS) $(LIB_KFILE) -lkonqsidebarplugin


globaladddir   = $(kde_datadir)/konqsidebartng/add
globaladd_DATA = news_add.desktop

kde_kcfg_DATA = konq_sidebarnews.kcfg



############################
#
# i18n
#
messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/konqsidebar_news.pot

