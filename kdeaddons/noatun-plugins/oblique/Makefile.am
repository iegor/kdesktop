SUBDIRS = . schemas

INCLUDES= -I$(top_srcdir)/noatun/library $(all_includes)
kde_module_LTLIBRARIES = noatun_oblique.la

noatun_oblique_la_SOURCES = base.cpp kbuffer.cpp query.cpp oblique.cpp tree.cpp file.cpp selector.cpp menu.cpp cmodule.cpp kdatacollection.cpp view.cpp

noatun_oblique_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatun_oblique_la_LIBADD = $(LIB_KIO) -lnoatun $(BERKELEY_DB_LIBS)

noatun_oblique_la_METASOURCES = AUTO

noatundata_DATA = oblique.plugin
noatundatadir = $(kde_datadir)/noatun

rc_DATA = obliqueui.rc
rcdir = $(kde_datadir)/noatun

