INCLUDES= $(all_includes) $(SDL_CFLAGS)

METASOURCES = AUTO

kde_module_LTLIBRARIES = noatunnexscope.la

noatunnexscope_la_SOURCES = noatunplugin.cpp
noatunnexscope_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatunnexscope_la_LIBADD = $(LIB_KFILE) -lnoatun

bin_PROGRAMS = nexscope.bin

#renderers.cpp must be first for enable-final
nexscope_bin_SOURCES = renderers.cpp bitmappool.cpp nex.cpp output.cpp input.cpp gui.cpp regionwidget.cpp convolve.c
nexscope_bin_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(SDL_LIBS)
nexscope_bin_LDADD = $(LIB_KDEUI) -lm  -lnoatun $(SDL_LIBS)

noatun_DATA = nexscope.plugin
noatundir = $(kde_datadir)/noatun

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/nexscope.pot
