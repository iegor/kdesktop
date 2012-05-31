
INCLUDES = $(all_includes)

kde_module_LTLIBRARIES = libphotobook.la

libphotobook_la_SOURCES = photobook.cpp

libphotobook_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
libphotobook_la_LIBADD = $(LIB_KFILE) $(LIB_KPARTS)

partrcdir = $(kde_servicesdir)
partrc_DATA = photobook.desktop

METASOURCES = AUTO

KDE_ICON = photobook

partdir = $(kde_datadir)/photobook
part_DATA = photobookui.rc
