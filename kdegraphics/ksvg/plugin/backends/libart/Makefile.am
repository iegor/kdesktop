kde_module_LTLIBRARIES = libksvgrendererlibart.la

libksvgrendererlibart_la_SOURCES = BezierPathLibart.cpp GlyphTracerLibart.cpp LibartCanvas.cpp LibartCanvasItems.cpp LibartCanvasFactory.cpp 
libksvgrendererlibart_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
libksvgrendererlibart_la_LIBADD = ../../../libksvg.la
libksvgrendererlibart_la_METASOURCES = AUTO

kde_services_DATA = ksvglibartcanvas.desktop

KDE_CXXFLAGS = $(USE_EXCEPTIONS)
INCLUDES = $(LIBART_CFLAGS) $(FREETYPE_CFLAGS) $(FONTCONFIG_CFLAGS) -I$(top_srcdir)/ksvg/dom -I$(top_srcdir)/ksvg/impl -I$(top_srcdir)/ksvg/ecma -I$(top_srcdir)/ksvg/core -I$(top_srcdir)/ksvg/impl/libs/art_support -I$(top_srcdir)/ksvg/impl/libs/libtext2path/src $(KDE_INCLUDES) $(all_includes)
