# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/katapult/common $(all_includes)

# header files
noinst_HEADERS = glassdisplay.h

# use automoc
METASOURCES = AUTO

# our plugin
kde_module_LTLIBRARIES =  katapult_glassdisplay.la
katapult_glassdisplay_la_SOURCES = glassdisplay.cpp
katapult_glassdisplay_la_LDFLAGS = -module $(KDE_RPATH) $(KDE_PLUGIN) $(all_libraries)
katapult_glassdisplay_la_LIBADD = $(LIB_KDEUI) $(top_builddir)/katapult/common/libkatapult.la
kde_services_DATA = katapult_glassdisplay.desktop
