# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/katapult/common $(all_includes)

# these are the headers for your project
noinst_HEADERS = katapult.h katapultsettings.h katapultconfigdlg.h \
	katapultiface.h
	

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

KDE_ICON = katapult

bin_PROGRAMS = katapult

# the application source, library search path, and link libraries
katapult_SOURCES = confgeneral.ui confcatalogs.ui confdisplay.ui main.cpp \
	katapult.cpp katapultsettings.cpp katapultconfigdlg.cpp katapultiface.skel
katapult_LDFLAGS = $(KDE_RPATH) $(all_libraries)
katapult_LDADD = $(LIB_KDEUI) $(LIB_KIO) $(top_builddir)/katapult/common/libkatapult.la

# this is where the desktop file will go 
xdg_apps_DATA = katapult.desktop
