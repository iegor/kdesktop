INCLUDES= -I$(kde_includes)/arts $(all_includes)

kde_module_LTLIBRARIES = noatunpitchablespeed.la
noatunpitchablespeed_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatunpitchablespeed_la_LIBADD = $(LIB_KFILE) -lnoatun -lm -lartsmodules

noatunpitchablespeed_la_SOURCES = pitchablespeed.cpp speedtuner.ui
noatunpitchablespeed_la_METASOURCES = AUTO
noinst_HEADERS = pitchablespeed.h

noatun_DATA = pitchablespeed.plugin
noatundir = $(kde_datadir)/noatun

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/pitchablespeed.pot
