INCLUDES= -I$(top_srcdir)/noatun/library -I$(kde_includes)/arts $(all_includes)

kde_module_LTLIBRARIES = noatunsimple.la

noatunsimple_la_SOURCES = propertiesdialog.ui noatunui.cpp userinterface.cpp
noatunsimple_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
noatunsimple_la_LIBADD =  $(top_builddir)/noatun/library/libnoatun.la \
			  $(top_builddir)/noatun/library/libnoatuncontrols.la \
			 -lkmedia2_idl -lsoundserver_idl -lartskde

noatunsimple_la_METASOURCES = AUTO

noinst_HEADERS = propertiesdialog.h userinterface.h

noatun_modules_simple_DATA = simple.plugin simpleui.rc
noatun_modules_simpledir = $(kde_datadir)/noatun
