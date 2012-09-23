## Makefile.am for kbabeldict

# this has all of the subdirectories that make will recurse into.  if
# there are none, comment this out
SUBDIRS = arguments accelerators context equations pluralforms xml \
not-translated whitespace length setfuzzy punctuation regexp

#pkgincludedir = $(includedir)/kbabel

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
#lib_LTLIBRARIES = libkbabeltools.la 

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)


# which sources should be compiled for kbabeldict
#libkbabeltools_la_SOURCES = toolaction.cpp kactionselector.cpp toolselectionwidget.cpp
#libkbabeltools_la_LIBADD =  $(LIB_KDECORE) $(LIB_KIO)
#libkbabeltools_la_LDFLAGS = $(all_libraries) -module -version-info 1:0:0 -no-undefined

# these are the headers for your project
#noinst_HEADERS   =  kactionselector.h toolselectionwidget.h
#pkginclude_HEADERS = toolaction.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

# KDE_OPTIONS = nofinal
#rcdir = $(kde_datadir)/kbabel
#rc_DATA = kbabelui.rc

#api:
#	mkdir -p API && kdoc -d API -u $$PWD/API -p  -lkdeui -lkdecore -lqt -ldcop *.h

#distclean-local:
#	rm -r -f API
    
