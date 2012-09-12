# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/katapult/common $(all_includes)

# header files
noinst_HEADERS = coverimage.h song.h amarokcatalog.h actionplaysong.h

# use automoc
METASOURCES = AUTO

# our plugin
kde_module_LTLIBRARIES = katapult_amarokcatalog.la
katapult_amarokcatalog_la_SOURCES = settings.ui amarokcatalog.cpp coverimage.cpp song.cpp actionplaysong.cpp
katapult_amarokcatalog_la_LDFLAGS = -module $(KDE_RPATH) $(KDE_PLUGIN) $(all_libraries)
katapult_amarokcatalog_la_LIBADD = $(LIB_KDEUI) $(LIB_KIO) $(top_builddir)/katapult/common/libkatapult.la
kde_services_DATA = katapult_amarokcatalog.desktop
