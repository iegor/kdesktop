kde_module_LTLIBRARIES = kded_daemonwatcher.la

INCLUDES = $(all_includes)
kded_daemonwatcher_la_SOURCES = watcher.cpp watcher.skel
# watcher.stub
kded_daemonwatcher_la_LDFLAGS = $(all_libraries) -module -avoid-version
kded_daemonwatcher_la_LIBADD = $(LIB_KSYCOCA) $(LIB_KDEUI)

METASOURCES = AUTO

servicesdir = $(kde_servicesdir)/kded
services_DATA = daemonwatcher.desktop
