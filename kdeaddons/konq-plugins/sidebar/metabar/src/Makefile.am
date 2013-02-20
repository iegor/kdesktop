INCLUDES = $(all_includes) -I$(kde_includes)/dom
METASOURCES = AUTO

kde_module_LTLIBRARIES = konqsidebar_metabar.la

KDE_ICON = AUTO

konqsidebar_metabar_la_SOURCES = metabar.cpp metabarwidget.cpp configdialog.cpp \
		serviceloader.cpp defaultplugin.cpp protocolplugin.cpp settingsplugin.cpp \
		remoteplugin.cpp httpplugin.cpp metabarfunctions.cpp
konqsidebar_metabar_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries) -lkonqsidebarplugin
konqsidebar_metabar_la_LIBADD = $(LIB_KIO) $(LIB_KHTML) $(LIB_KPARTS)

metabar_entrydir = $(kde_datadir)/konqsidebartng/entries
metabar_entry_DATA = metabar.desktop
metabar_adddir = $(kde_datadir)/konqsidebartng/add
metabar_add_DATA = metabar_add.desktop
metabar_datadir = $(kde_datadir)/metabar
metabar_data_DATA = iconsrc
metabar_themedir = $(kde_datadir)/metabar/themes/default
metabar_theme_DATA = default.css layout.html

noinst_HEADERS = metabarwidget.h protocolplugin.h defaultplugin.h \
		settingsplugin.h remoteplugin.h httpplugin.h metabarfunctions.h

messages:
	$(XGETTEXT) *.cpp *.h -o $(podir)/konqsidebar_metabar.pot

