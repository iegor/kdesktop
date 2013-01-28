## Makefile.am for html file meta info plugin

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# these are the headers for your project
noinst_HEADERS   = kfile_html.h

kde_module_LTLIBRARIES = kfile_html.la

kfile_html_la_SOURCES = kfile_html.cpp
kfile_html_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_html_la_LIBADD = $(LIB_KSYCOCA)

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) kfile_html.cpp -o $(podir)/kfile_html.pot

services_DATA = kfile_html.desktop
servicesdir = $(kde_servicesdir)
