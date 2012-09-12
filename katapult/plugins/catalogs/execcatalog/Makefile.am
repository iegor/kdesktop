# Copyright (C) 2007 Martin Meredith
# Copyright (C) 2006 Jonathan Riddell

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/katapult/common $(all_includes)

# header files
noinst_HEADERS = actionrun.h execcatalog.h command.h

# use automoc
METASOURCES = AUTO

KDE_ICON = AUTO

# our plugin
kde_module_LTLIBRARIES =  katapult_execcatalog.la
katapult_execcatalog_la_SOURCES = settings.ui execcatalog.cpp command.cpp actionrun.cpp
katapult_execcatalog_la_LDFLAGS = -module $(KDE_RPATH) $(KDE_PLUGIN) $(all_libraries)
katapult_execcatalog_la_LIBADD = $(LIB_KDEUI) $(LIB_KIO) $(top_builddir)/katapult/common/libkatapult.la
kde_services_DATA = katapult_execcatalog.desktop
