## Makefile.am for libkbabelcommon

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
noinst_LTLIBRARIES = libkbabelcommonui.la

# set the include path for X, qt and KDE. Put local paths before all_includes.
INCLUDES         = -I$(srcdir)/../common -I../common -I$(srcdir)/../kbabeldict -I../kbabeldict $(all_includes)

# which sources should be compiled 
libkbabelcommonui_la_SOURCES = klisteditor.ui context.cpp kactionselector.cpp \
			       toolselectionwidget.cpp toolaction.cpp \
			       finddialog.cpp roughtransdlg.cpp \
			       projectprefwidgets.cpp		\
			       projectpref.cpp		\
			       projectwizard.cpp	\
			       projectwizardwidget.ui	\
			       projectwizardwidget2.ui	\
			       cmdedit.cpp		\
			       diffpreferences.ui

libkbabelcommonui_la_LIBADD =  $(LIB_KIO) -lktexteditor ../common/libkbabelcommon.la ../kbabeldict/libkbabeldict.la

libkbabelcommonui_la_LDFLAGS = $(all_libraries)

# these are the headers for your project
noinst_HEADERS   =  context.h kactionselector.h finddialog.h \
		    roughtransdlg.h projectprefwidgets.h projectpref.h	\
		    cmdedit.h projectwizard.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

# we need to define our own service type for our plugins. It's data tool with a shortcut :-((
kde_servicetypes_DATA = kbabel_validator.desktop kbabel_tool.desktop
EXTRA_DIST = $(kde_servicetypes_DATA)

context.lo: ../common/kbprojectsettings.h
projectpref.lo: ../common/kbprojectsettings.h
projectprefwidgets.lo: ../common/kbprojectsettings.h
