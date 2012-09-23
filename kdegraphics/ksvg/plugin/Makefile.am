SUBDIRS = backends .

INCLUDES = $(FREETYPE_CFLAGS) -I$(top_srcdir)/ksvg -I$(top_srcdir)/ksvg/dom -I$(top_srcdir)/ksvg/impl -I$(top_srcdir)/ksvg/ecma -I$(top_srcdir)/ksvg/core -I$(top_srcdir)/ksvg/impl/libs/libtext2path/src $(all_includes)
METASOURCES = AUTO
KDE_CXXFLAGS = $(USE_EXCEPTIONS)

kde_module_LTLIBRARIES = libksvgplugin.la svgthumbnail.la

libksvgplugin_la_SOURCES = ksvg_plugin.cpp ksvg_factory.cpp ksvg_widget.cpp
libksvgplugin_la_LIBADD = ../libksvg.la
libksvgplugin_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

ksvginclude_HEADERS = ksvg_plugin.h
ksvgincludedir = $(includedir)/ksvg

kde_services_DATA = ksvgplugin.desktop

svgthumbnail_la_SOURCES = svgcreator.cpp
svgthumbnail_la_LIBADD = ../libksvg.la
svgthumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

services_DATA = svgthumbnail.desktop
servicesdir = $(kde_servicesdir)

partdir = $(kde_datadir)/ksvg
part_DATA = ksvgplugin.rc

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/ksvgplugin.pot
