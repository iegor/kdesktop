# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir) $(all_includes) $(AVAHI_CFLAGS)

# these are the headers for your project

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

lib_LTLIBRARIES =  libkdnssd.la

libkdnssd_la_SOURCES = remoteservice.cpp responder.cpp servicebase.cpp \
				settings.kcfgc publicservice.cpp query.cpp domainbrowser.cpp servicebrowser.cpp
dnssdincludedir = $(includedir)/dnssd
noinst_HEADERS = domainbrowser.h query.h remoteservice.h \
	publicservice.h servicebase.h servicebrowser.h settings.h sdevent.h
libkdnssd_la_CXXFLAGS = $(INCLUDES)
libkdnssd_la_LIBADD = $(LIB_KDECORE) $(AVAHI_LIBS)
libkdnssd_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -version-info 1:0

#kde_kcfg_DATA = kcm_kdnssd.kcfg

include ../admin/Doxyfile.am
