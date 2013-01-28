INCLUDES= $(all_includes)
kde_module_LTLIBRARIES = noatunluckytag.la

noatunluckytag_la_SOURCES = luckytag.cpp parsedmp3filename.cpp
noatunluckytag_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatunluckytag_la_LIBADD = $(LIB_KFILE) -lnoatun -lnoatuntags

noatunluckytag_la_METASOURCES = AUTO

noatun_DATA = luckytag.plugin
noatundir = $(kde_datadir)/noatun
