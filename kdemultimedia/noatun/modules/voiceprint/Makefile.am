INCLUDES= -I$(top_srcdir)/noatun/library $(all_includes)
kde_module_LTLIBRARIES = noatun_voiceprint.la

noatun_voiceprint_la_SOURCES = voiceprint.cpp prefs.cpp

noatun_voiceprint_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatun_voiceprint_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/library/libnoatun.la -lm

noatun_voiceprint_la_METASOURCES = AUTO

noinst_HEADERS = voiceprint.h prefs.h

noatun_modules_voiceprint_DATA = voiceprint.plugin
noatun_modules_voiceprintdir = $(kde_datadir)/noatun
