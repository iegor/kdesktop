INCLUDES = $(all_includes)

noinst_HEADERS   = kdeaccountsformat.h
kde_module_LTLIBRARIES = kabcformat_kdeaccounts.la
kabcformat_kdeaccounts_la_SOURCES = kdeaccountsformat.cpp
kabcformat_kdeaccounts_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kabcformat_kdeaccounts_la_LIBADD  = -lkabc

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kabcformat_kdeaccounts.pot

linkdir		= $(kde_datadir)/kabc/formats
link_DATA	= kdeaccountsplugin.desktop
EXTRA_DIST	= $(link_DATA) README
