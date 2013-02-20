INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = kwin_cde_config.la

kwin_cde_config_la_SOURCES = config.cpp
kwin_cde_config_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kwin_cde_config_la_LIBADD  = $(LIB_KDEUI)

METASOURCES = AUTO
noinst_HEADERS = config.h

lnkdir = $(kde_datadir)/kwin/


###KMAKE-start (don't edit or delete this block)

###KMAKE-end
