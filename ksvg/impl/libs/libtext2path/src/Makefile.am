lib_LTLIBRARIES = libtext2path.la

KDE_CXXFLAGS = $(USE_EXCEPTIONS)
INCLUDES = $(FRIBIDI_CFLAGS) $(FREETYPE_CFLAGS) $(FONTCONFIG_CFLAGS) $(all_includes)

libtext2path_includedir=$(includedir)/libtext2path-0.1
libtext2path_include_HEADERS = BezierPath.h Glyph.h GlyphTracer.h

libtext2path_la_SOURCES = Affine.cpp Rectangle.cpp Font.cpp Glyph.cpp GlyphTracer.cpp Converter.cpp QtUnicode.cpp
libtext2path_la_LDFLAGS = $(FRIBIDI_LIBS) $(FONTCONFIG_LIBS)
