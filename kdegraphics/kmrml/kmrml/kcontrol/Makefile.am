LIB_KMRMLSTUFF = $(top_builddir)/kmrml/kmrml/lib/libkmrmlstuff.la

kde_module_LTLIBRARIES = kcm_kmrml.la

kcm_kmrml_la_SOURCES = kcmkmrml.cpp mainpage.cpp indexer.cpp serverconfigwidget.ui indexcleaner.cpp
kcm_kmrml_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
kcm_kmrml_la_LIBADD = $(LIB_KMRMLSTUFF) $(LIB_KIO)
INCLUDES= -I$(top_srcdir)/kmrml/kmrml/lib $(all_includes)
 
kcm_kmrml_la_METASOURCES = AUTO

noinst_HEADERS = kcmkmrml.h mainpage.h serverconfigwidget.h indexer.h indexcleaner.h

xdg_apps_DATA = kcmkmrml.desktop

#check_PROGRAMS = indextest
#indextest_SOURCES = indextest.cpp indexer.cpp
#indextest_LDADD = $(LIB_KMRMLSTUFF) $(LIB_KDECORE)
#indextest_LDFLAGS = $(all_libraries)



#pics_DATA = play.png
#picsdir = $(kde_datadir)/kcontrol/pics

