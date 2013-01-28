# this has all of the subdirectories that make will recurse into.  if
# there are none, comment this out
SUBDIRS = . pics

# set the include path for X, qt and KDE
INCLUDES = -I$(kde_includes)/kio -I$(kde_includes)/arts $(all_includes)        

# these are the headers for your project
noinst_HEADERS = 
# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

kde_module_LTLIBRARIES = konqsidebar_mediaplayer.la

# the Part's source, library search path, and link libraries
konqsidebar_mediaplayer_la_SOURCES = mediawidget_skel.ui engine.cpp player.cpp mediawidget.cpp mediaplayer.cpp controls.cpp
konqsidebar_mediaplayer_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
konqsidebar_mediaplayer_la_LIBADD  = $(LIB_KPARTS) $(LIB_KFILE) -lkonqsidebarplugin -lqtmcop -lkmedia2_idl -lsoundserver_idl -lartskde 

globaladddir   = $(kde_datadir)/konqsidebartng/add
globaladd_DATA = mplayer_add.desktop

appsdir = $(kde_appsdir)/.hidden
apps_DATA = mediaplayerplugin.desktop

mediawidget_skel.ui: mediawidget_skel_designer.ui
	perl -p -e "s/QSlider/L33tSlider/" mediawidget_skel_designer.ui >mediawidget_skel.ui

extendedclean:
	rm mediawidget_skel.*

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/konqsidebar_mediaplayer.pot
