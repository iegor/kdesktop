
INCLUDES = -I$(srcdir)/../../common $(all_includes)
METASOURCES = AUTO

kde_module_LTLIBRARIES = pothumbnail.la

pothumbnail_la_SOURCES = pothumbcreator.cpp
pothumbnail_la_LIBADD = ../../common/libkbabelcommon.la 
pothumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

noinst_HEADERS = pothumbcreator.h 

services_DATA = pothumbnail.desktop 
servicesdir = $(kde_servicesdir)

