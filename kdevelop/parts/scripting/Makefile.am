INCLUDES = -I$(top_srcdir)/lib/interfaces -I$(top_srcdir)/lib/util $(all_includes)

kde_module_LTLIBRARIES = libkdevscripting.la
libkdevscripting_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkdevscripting_la_LIBADD = $(top_builddir)/lib/libkdevelop.la

libkdevscripting_la_SOURCES = scriptingpart.cpp scriptingglobalconfigbase.ui \
	scriptingglobalconfig.cpp

METASOURCES = AUTO

appsharedir = $(kde_datadir)/kdevscripting
appshare_DATA = 

servicedir = $(kde_servicesdir)
service_DATA = kdevscripting.desktop

rcdir = $(kde_datadir)/kdevscripting
rc_DATA = kdevscripting.rc
noinst_HEADERS = scriptingglobalconfig.h  scriptingpart.h
