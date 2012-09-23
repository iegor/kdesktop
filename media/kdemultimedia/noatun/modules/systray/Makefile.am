INCLUDES= -I$(top_srcdir)/noatun/library $(all_includes)
kde_module_LTLIBRARIES = noatun_systray.la

noatun_systray_la_SOURCES = systray.cpp noatunui.cpp kitsystemtray.cpp cmodule.cpp \
	 yhconfig.kcfgc yhconfigwidget.ui


noatun_systray_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatun_systray_la_LIBADD = $(LIB_KFILE) $(top_builddir)/noatun/library/libnoatun.la

noatun_systray_la_METASOURCES = AUTO

noinst_HEADERS = systray.h kitsystemtray.h cmodule.h

noatun_modules_systray_DATA = systray.plugin systrayui.rc yhconfig.kcfg
noatun_modules_systraydir = $(kde_datadir)/noatun
