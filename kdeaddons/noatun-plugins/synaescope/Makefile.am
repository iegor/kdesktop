INCLUDES= $(all_includes) $(SDL_CFLAGS)

METASOURCES = AUTO

kde_module_LTLIBRARIES = noatunsynaescope.la

noatunsynaescope_la_SOURCES = synaescope.cpp cmodule.cpp
noatunsynaescope_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatunsynaescope_la_LIBADD = $(LIB_KFILE) -lnoatun -lm

bin_PROGRAMS = noatunsynaescope.bin

noatunsynaescope_bin_SOURCES = core.cpp main.cpp sdlwrap.cpp ui.cpp
noatunsynaescope_bin_LDFLAGS = $(all_libraries) $(KDE_RPATH)
noatunsynaescope_bin_LDADD = $(LIB_KDEUI) -lnoatun $(SDL_LIBS)

noatun_DATA = synaescope.plugin
noatundir = $(kde_datadir)/noatun

KDE_ICON = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/synaescope.pot
