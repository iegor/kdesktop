# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/katapult/common $(all_includes)

# header files
noinst_HEADERS = bookmarkcatalog.h bookmark.h actionopenbookmark.h \
	mozillabookmark.h

# use automoc
METASOURCES = AUTO

# our plugin
kde_module_LTLIBRARIES =  katapult_bookmarkcatalog.la
katapult_bookmarkcatalog_la_SOURCES = settings.ui bookmarkcatalog.cpp \
	bookmark.cpp actionopenbookmark.cpp mozillabookmark.cpp
katapult_bookmarkcatalog_la_LDFLAGS = -module $(KDE_RPATH) $(KDE_PLUGIN) $(all_libraries)
katapult_bookmarkcatalog_la_LIBADD = $(LIB_KDEUI) $(LIB_KIO) $(top_builddir)/katapult/common/libkatapult.la
kde_services_DATA = katapult_bookmarkcatalog.desktop
