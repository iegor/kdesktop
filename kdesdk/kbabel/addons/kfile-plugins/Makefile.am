## Makefile.am for po file meta info plugin

# set the include path for X, qt and KDE
INCLUDES = -I$(srcdir)/../../common/ $(all_includes)

# these are the headers for the project
noinst_HEADERS   = kfile_po.h

kde_module_LTLIBRARIES = kfile_po.la

kfile_po_la_SOURCES = kfile_po.cpp
kfile_po_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_po_la_LIBADD  = ../../common/libkbabelcommon.la $(LIB_KIO)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) kfile_po.cpp -o $(podir)/kfile_po.pot

services_DATA = kfile_po.desktop
servicesdir = $(kde_servicesdir)
