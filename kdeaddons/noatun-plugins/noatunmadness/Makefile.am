INCLUDES= $(all_includes)

kde_module_LTLIBRARIES = noatunmadness.la
noatunmadness_la_SOURCES = madness.cpp
noatunmadness_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatunmadness_la_LIBADD = $(LIB_KFILE) -lnoatun -lm
noatunmadness_la_METASOURCES = AUTO

noatun_DATA = madness.plugin
noatundir = $(kde_datadir)/noatun
