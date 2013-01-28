INCLUDES = $(all_includes) 

kde_module_LTLIBRARIES = libkaddrbk_geo_xxport.la

servicedir  = $(kde_servicesdir)/kaddressbook
service_DATA = geo_xxport.desktop

libkaddrbk_geo_xxport_la_SOURCES = geo_xxport.cpp
libkaddrbk_geo_xxport_la_LDFLAGS = -module $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries)
libkaddrbk_geo_xxport_la_LIBADD = $(LIB_KDEUI) -lkabc -lkabinterfaces
libkaddrbk_geo_xxport_la_METASOURCES = AUTO

rc_DATA = geo_xxportui.rc
rcdir = $(kde_datadir)/kaddressbook

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/libkaddrbk_geo_xxport.pot
