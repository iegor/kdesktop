INCLUDES = $(FREETYPE_CFLAGS) -I$(top_srcdir)/ksvg/dom -I$(top_srcdir)/ksvg/impl -I$(top_srcdir)/ksvg/core -I$(top_srcdir)/ksvg/data -I$(top_srcdir)/ksvg/impl/libs/libtext2path/src $(all_includes)
METASOURCES = AUTO
KDE_CXXFLAGS = $(USE_EXCEPTIONS)

noinst_LTLIBRARIES = libksvgecma.la

libksvgecma_la_SOURCES = ksvg_scriptinterpreter.cpp ksvg_ecma.cpp ksvg_helper.cpp ksvg_ecmaeventlistener.cpp ksvg_window.cpp
