INCLUDES    = $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES   = libkimgallery.la
libkimgallery_la_SOURCES = imgalleryplugin.cpp imgallerydialog.cpp
libkimgallery_la_LIBADD  = -lkonq
libkimgallery_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

iconviewdir   = $(kde_datadir)/konqiconview/kpartplugins
iconview_DATA = kimgalleryplugin.rc kimgalleryplugin.desktop

listviewdir   = $(kde_datadir)/konqlistview/kpartplugins
listview_DATA = kimgalleryplugin.rc kimgalleryplugin.desktop

appsdir = $(kde_appsdir)/.hidden
apps_DATA = kimgalleryplugin.desktop 

KDE_ICON = imagegallery

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/imgalleryplugin.pot
