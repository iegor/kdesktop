kde_module_LTLIBRARIES = noatundub.la

INCLUDES = $(all_includes)

noatundub_la_SOURCES = dubprefs.ui dubplaylistitem.cpp dubplaylist.cpp \
fileselectorwidget.cpp dubview.cpp dubapp.cpp dub.cpp dubconfigmodule.cpp

EXTRA_DIST = dubui.rc dub.plugin BUGS \
 dub.h dubdoc.h dubview.h fileselectorwidget.h dubplaylist.h dubplaylistitem.h 

noatundub_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatundub_la_LIBADD = $(LIB_KIO) -lnoatun

noatun_DATA = dub.plugin
noatundir = $(kde_datadir)/noatun

METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/dub.pot
