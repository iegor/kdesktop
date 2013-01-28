INCLUDES    =  $(all_includes)
METASOURCES = AUTO

subdirs=src templates

#KDE_ICON = filetemplates

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = katefiletemplates.la

katefiletemplates_la_SOURCES = filetemplates.cpp
katefiletemplates_la_LIBADD = -lkateinterfaces -lktexteditor $(LIB_KNEWSTUFF)
katefiletemplates_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

pluginsdir = $(kde_datadir)/kate/plugins/katefiletemplates
plugins_DATA = ui.rc

syntaxdir = $(kde_datadir)/katepart/syntax
syntax_DATA = katetemplate.xml

kde_services_DATA = katefiletemplates.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/katefiletemplates.pot

