INCLUDES=$(all_includes)

noinst_LTLIBRARIES = libkeduca.la
libkeduca_la_SOURCES = kgallerydialogbase.ui fileread.cpp kgallerydialog.cpp
libkeduca_la_LDFLAGS = $(all_libraries) $(KDE_RPATH)
libkeduca_la_LIBADD  = -lkdeprint
noinst_HEADERS = fileread.h kgallerydialog.h kgallerydialogbase.h

METASOURCES=AUTO
include ../../admin/Doxyfile.am
