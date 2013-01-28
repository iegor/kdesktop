## Makefile.am for folder file meta info plugin

INCLUDES         = $(all_includes)

# these are the headers for your project
noinst_HEADERS   = kfile_folder.h

kde_module_LTLIBRARIES = kfile_folder.la

kfile_folder_la_SOURCES = kfile_folder.cpp
kfile_folder_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_folder_la_LIBADD = $(LIB_KIO)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

services_DATA = kfile_folder.desktop
servicesdir = $(kde_servicesdir)

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kfile_folder.pot
