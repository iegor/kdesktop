## Makefile.am for klatin

# this has all of the subdirectories that make will recurse into.  if
# there are none, comment this out
SUBDIRS = . data icons

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = klatin

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# the library search path.
klatin_LDFLAGS = $(KDE_RPATH) $(all_libraries)

# the libraries to link against.
klatin_LDADD   = $(LIB_KFILE) $(top_builddir)/libkdeedu/kdeeducore/libkdeeducore.la

# which sources should be compiled for klatin
klatin_SOURCES =	main.cpp klatin.cpp klatingrammar.cpp klatinverbs.cpp klatinvocab.cpp \
	klatinchoose.ui settings.kcfgc klatingrammarwidget.ui klatinvocabwidget.ui klatinverbswidget.ui \
	klatinresultsdialog.ui klatinsettings_vocabpage.ui

# these are the headers for your project
noinst_HEADERS   = klatin.h klatinverbs.h klatinvocab.h klatingrammar.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/klatin.pot

xdg_apps_DATA = klatin.desktop
kde_kcfg_DATA = klatin.kcfg

# this is where the XML-GUI resource file goes
rcdir = $(kde_datadir)/klatin
rc_DATA = klatinui.rc

