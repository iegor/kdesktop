
xdg_apps_DATA = kpackage.desktop

# claim, which subdirectories you want to install
SUBDIRS = pics toolbar icon

bin_PROGRAMS =  kpackage

# Which sources should be compiled for kpackage.
kpackage_SOURCES = kpackage.cpp managementWidget.cpp packageDisplay.cpp \
	  packageProperties.cpp  findf.cpp search.cpp \
	  options.cpp pkgOptions.cpp \
	  packageInfo.cpp  cache.cpp main.cpp utils.cpp kio.cpp \
	  debInterface.cpp debDpkgInterface.cpp debAptInterface.cpp \
	  updateLoc.cpp procbuf.cpp kplview.cpp \
	  pkgInterface.cpp  rpmInterface.cpp kissInterface.cpp \
	  slackInterface.cpp fbsdInterface.cpp gentooInterface.cpp \
	  kpPty.cpp kpTerm.cpp

kpackage_METASOURCES = AUTO

# the library search path
kpackage_LDFLAGS =  $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
kpackage_LDADD   =  $(LIB_KFILE) $(LIBINTL) $(LIBZ)  $(LIBUTEMPTER)

INCLUDES = $(all_includes)

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kpackage.pot

rcdir = $(kde_datadir)/kpackage
rc_DATA = kpackageui.rc
 
