kde_module_LTLIBRARIES = libksvgrendereragg.la

# Damn agg2 has no configuration querying possibility!
AGG_CFLAGS = -I/cvs/agg2/include/
AGG_LIBS = /cvs/agg2/src/libagg.a

libksvgrendereragg_la_SOURCES = BezierPathAgg.cpp GlyphTracerAgg.cpp AggCanvas.cpp AggCanvasItems.cpp AggCanvasFactory.cpp
libksvgrendereragg_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
libksvgrendereragg_la_LIBADD = ../../../libksvg.la $(AGG_LIBS)
libksvgrendereragg_la_METASOURCES = AUTO

kde_services_DATA = ksvgaggcanvas.desktop

KDE_CXXFLAGS = $(USE_EXCEPTIONS)
INCLUDES = $(KDE_INCLUDES) $(AGG_CFLAGS) $(FREETYPE_CFLAGS) $(FONTCONFIG_CFLAGS) -I$(top_srcdir)/ksvg/dom -I$(top_srcdir)/ksvg/impl -I$(top_srcdir)/ksvg/ecma -I$(top_srcdir)/ksvg/core -I$(top_srcdir)/ksvg/impl/libs/art_support -I$(top_srcdir)/ksvg/impl/libs/libtext2path/src $(all_includes)
