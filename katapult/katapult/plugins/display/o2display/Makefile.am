# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/katapult/common $(all_includes)

# header files
noinst_HEADERS = o2display.h

# use automoc
METASOURCES = AUTO

# our plugin
kde_module_LTLIBRARIES =  katapult_o2display.la
katapult_o2display_la_SOURCES = o2display.cpp
katapult_o2display_la_LDFLAGS = -module $(KDE_RPATH) $(KDE_PLUGIN) $(all_libraries)
katapult_o2display_la_LIBADD = $(LIB_KDEUI) $(top_builddir)/katapult/common/libkatapult.la
kde_services_DATA = katapult_o2display.desktop
