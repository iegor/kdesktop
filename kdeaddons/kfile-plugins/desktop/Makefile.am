## Makefile.am for .desktop file meta info plugin

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# these are the headers for your project
noinst_HEADERS   = kfile_desktop.h

kde_module_LTLIBRARIES = kfile_desktop.la

kfile_desktop_la_SOURCES = kfile_desktop.cpp
kfile_desktop_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_desktop_la_LIBADD = $(LIB_KIO)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kfile_desktop.pot

services_DATA = kfile_desktop.desktop
servicesdir = $(kde_servicesdir)
