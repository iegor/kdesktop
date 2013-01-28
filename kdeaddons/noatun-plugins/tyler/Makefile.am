INCLUDES= $(all_includes) $(SDL_CFLAGS)

METASOURCES = AUTO

kde_module_LTLIBRARIES = noatuntyler.la

noatuntyler_la_SOURCES = tyler.cpp
noatuntyler_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatuntyler_la_LIBADD = $(LIB_KFILE) -lnoatun -lm

bin_PROGRAMS = noatuntyler.bin

noatuntyler_bin_SOURCES = main.cpp file.cpp compute.c display.c renderer.c
noatuntyler_bin_LDFLAGS = $(all_libraries) $(KDE_RPATH)
noatuntyler_bin_LDADD = $(LIB_KDECORE) -lm $(SDL_LIBS)

noatun_DATA = tyler.plugin tylerstates
noatundir = $(kde_datadir)/noatun

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/tyler.pot
