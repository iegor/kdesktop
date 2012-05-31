noinst_LTLIBRARIES = libksvgcore.la

libksvgcore_la_SOURCES = KSVGLoader.cpp KSVGCanvas.cpp KSVGReader.cc KSVGTextChunk.cpp CanvasFactory.cpp CanvasItems.cpp KSVGHelper.cpp DocumentFactory.cpp
libksvgcore_la_METASOURCES = AUTO

servicetypedir = $(kde_servicetypesdir)
servicetype_DATA = ksvgrenderer.desktop

ksvginclude_HEADERS = KSVGCanvas.h CanvasItems.h CanvasItem.h CanvasFactory.h DocumentFactory.h
ksvgincludedir = $(includedir)/ksvg

KDE_CXXFLAGS = $(USE_EXCEPTIONS)
INCLUDES = $(FREETYPE_CFLAGS) $(FONTCONFIG_CFLAGS) -I$(top_srcdir)/ksvg/dom -I$(top_srcdir)/ksvg/impl -I$(top_srcdir)/ksvg/ecma -I$(top_srcdir)/ksvg/impl/libs/art_support -I$(top_srcdir)/ksvg/impl/libs/libtext2path/src $(all_includes)

KDE_OPTIONS = nofinal

