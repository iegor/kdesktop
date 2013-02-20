INCLUDES = $(FREETYPE_CFLAGS) -I$(top_srcdir)/ksvg -I$(top_srcdir)/ksvg/dom -I$(top_srcdir)/ksvg/impl -I$(top_srcdir)/ksvg/ecma -I$(top_srcdir)/ksvg/core -I$(top_srcdir)/ksvg/impl/libs/libtext2path/src $(all_includes)
METASOURCES = AUTO
KDE_CXXFLAGS = $(USE_EXCEPTIONS)

bin_PROGRAMS = svgdisplay printnodetest

svgdisplay_SOURCES = SVGTestWidget.cc svgdisplay.cc
svgdisplay_LDADD =  $(LIB_KDECORE) -lfribidi ../../libksvg.la
svgdisplay_LDFLAGS = $(KDE_RPATH) $(all_libraries)

printnodetest_SOURCES = printnodetest.cpp
printnodetest_LDADD =  $(LIB_KDECORE) -lfribidi ../../libksvg.la
printnodetest_LDFLAGS = $(KDE_RPATH) $(all_libraries)
