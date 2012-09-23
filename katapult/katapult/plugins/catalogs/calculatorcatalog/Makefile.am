# Copyright (C) 2005  Tobi Vollebregt

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/katapult/common $(all_includes)

# header files
noinst_HEADERS = calculatorcatalog.h expression.h actionevalexpr.h 

# use automoc
METASOURCES = AUTO

# install xcalc icon
KDE_ICON = xcalc checkmark no

# our plugin
kde_module_LTLIBRARIES =  katapult_calculatorcatalog.la
katapult_calculatorcatalog_la_SOURCES = settings.ui calculatorcatalog.cpp expression.cpp actionevalexpr.cpp parser.cpp
katapult_calculatorcatalog_la_LDFLAGS = -module $(KDE_RPATH) $(KDE_PLUGIN) $(all_libraries)
katapult_calculatorcatalog_la_LIBADD = $(LIB_KDEUI) $(LIB_KIO) $(top_builddir)/katapult/common/libkatapult.la
kde_services_DATA = katapult_calculatorcatalog.desktop
