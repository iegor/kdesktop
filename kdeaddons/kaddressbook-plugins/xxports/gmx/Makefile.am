INCLUDES = $(all_includes) 

kde_module_LTLIBRARIES = libkaddrbk_gmx_xxport.la

servicedir = $(kde_servicesdir)/kaddressbook
service_DATA = gmx_xxport.desktop

libkaddrbk_gmx_xxport_la_SOURCES = gmx_xxport.cpp
libkaddrbk_gmx_xxport_la_LDFLAGS = -module $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries) -no-undefined
libkaddrbk_gmx_xxport_la_LIBADD = $(LIB_KDEUI) -lkabc -lkabinterfaces
libkaddrbk_gmx_xxport_la_METASOURCES = AUTO

rc_DATA = gmx_xxportui.rc
rcdir = $(kde_datadir)/kaddressbook

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/libkaddrbk_gmx_xxport.pot

