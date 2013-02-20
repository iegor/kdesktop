SUBDIRS= kcmkuick

INCLUDES    = $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = libkuickplugin.la

# This is all standard.  Remove the LIB_KHTML reference if you are not
# using the KHTML Part
libkuickplugin_la_SOURCES = kuick_plugin.cpp kdirmenu.cpp kmetamenu.cpp kimcontactmenu.cpp
libkuickplugin_la_LIBADD = $(LIB_KABC) -lkimproxy -lkonq
libkuickplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

pluginsdir   = $(kde_servicesdir)
plugins_DATA = kuick_plugin.desktop

appsdir = $(kde_appsdir)/.hidden
apps_DATA = kuickplugin.desktop 

# Install the .rc file in the Part's directory (in this case, the part
# is KHTMLPart)

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kuick_plugin.pot
noinst_HEADERS =  kimcontactmenu.h
