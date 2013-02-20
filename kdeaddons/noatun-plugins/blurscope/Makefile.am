INCLUDES= $(all_includes) $(SDL_CFLAGS)

kde_module_LTLIBRARIES = noatunblurscope.la
noatunblurscope_la_SOURCES = sdlscope.cpp scopedisplayer.cpp
noatunblurscope_la_LDFLAGS = -module -avoid-version -no-undefined $(all_libraries)
noatunblurscope_la_LIBADD = $(LIB_KFILE) -lnoatun -lm $(SDL_LIBS)
noatunblurscope_la_METASOURCES = AUTO

noatun_DATA = blurscope.plugin
noatundir = $(kde_datadir)/noatun
