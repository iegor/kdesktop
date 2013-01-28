## Makefile.am for text file meta info plugin

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

noinst_HEADERS   = kfile_txt.h

kde_module_LTLIBRARIES = kfile_txt.la

kfile_txt_la_SOURCES = kfile_txt.cpp
kfile_txt_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_txt_la_LIBADD = $(LIB_KIO)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kfile_txt.pot

services_DATA = kfile_txt.desktop
servicesdir = $(kde_servicesdir)
