INCLUDES= -I$(top_srcdir)/noatun/library $(all_includes)
kde_module_LTLIBRARIES = noatun_splitplaylist.la

noatun_splitplaylist_la_SOURCES = splitplaylist.cpp playlist.cpp view.cpp find.cpp

noatun_splitplaylist_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatun_splitplaylist_la_LIBADD = $(LIB_KIO) $(top_builddir)/noatun/library/libnoatun.la

noatun_splitplaylist_la_METASOURCES = AUTO

noinst_HEADERS = playlist.h view.h find.h

noatun_modules_splitplaylist_DATA = splitplaylist.plugin
noatun_modules_splitplaylistdir = $(kde_datadir)/noatun

rc_DATA = splui.rc
rcdir = $(kde_datadir)/noatun
