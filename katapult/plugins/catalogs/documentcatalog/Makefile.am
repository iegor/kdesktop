# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/katapult/common $(all_includes)

# header files
noinst_HEADERS = documentcatalog.h directory.h document.h actionopendocument.h

# use automoc
METASOURCES = AUTO

# our plugin
kde_module_LTLIBRARIES =  katapult_documentcatalog.la
katapult_documentcatalog_la_SOURCES = settings.ui documentcatalog.cpp \
		directory.cpp document.cpp actionopendocument.cpp
katapult_documentcatalog_la_LDFLAGS = -module $(KDE_RPATH) $(KDE_PLUGIN) $(all_libraries)
katapult_documentcatalog_la_LIBADD = $(LIB_KDEUI) $(LIB_KIO) $(top_builddir)/katapult/common/libkatapult.la
kde_services_DATA = katapult_documentcatalog.desktop
