INCLUDES= -I$(top_srcdir)/noatun/library $(all_includes)
kde_module_LTLIBRARIES = noatun_ui.la

noatun_ui_la_SOURCES = noatunui.cpp userinterface.cpp

noatun_ui_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatun_ui_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/library/libnoatun.la \
                        $(top_builddir)/noatun/library/libnoatuncontrols.la \
                        -lqtmcop -lkmedia2_idl -lsoundserver_idl

noatun_ui_la_METASOURCES = AUTO

noinst_HEADERS = userinterface.h

noatun_modules_ui_DATA = noatunui.plugin
noatun_modules_uidir = $(kde_datadir)/noatun
