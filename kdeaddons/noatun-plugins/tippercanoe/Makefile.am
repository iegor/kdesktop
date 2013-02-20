INCLUDES= $(all_includes) $(SDL_CFLAGS)

METASOURCES = AUTO

kde_module_LTLIBRARIES = noatuntippecanoe.la

noatuntippecanoe_la_SOURCES = synaescope.cpp
noatuntippecanoe_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatuntippecanoe_la_LIBADD = $(LIB_KFILE) -lnoatun -lm

bin_PROGRAMS = noatuntippecanoe.bin

noatuntippecanoe_bin_SOURCES = core.cpp main.cpp ui.cpp sdlwrap.cpp
noatuntippecanoe_bin_LDFLAGS = $(all_libraries) $(KDE_RPATH)
noatuntippecanoe_bin_LDADD = $(LIB_KDEUI) -lm $(SDL_LIBS)

noatun_DATA = tippecanoe.plugin
noatundir = $(kde_datadir)/noatun

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/tippecanoe.pot
