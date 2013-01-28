## Makefile.am for lnk file meta info plugin

INCLUDES         = $(all_includes)

bin_PROGRAMS = lnkforward

lnkforward_SOURCES = lnkforward.cpp
lnkforward_LDADD = $(LIB_KDECORE) $(LIB_KIO)
lnkforward_LDFLAGS = $(all_libraries) $(KDE_RPATH)
lnkforward_METASOURCES = lnkforward.moc

lnkforward_DATA = lnkforward.desktop
lnkforwarddir = $(kde_appsdir)/.hidden

# these are the headers for your project
noinst_HEADERS   = kfile_lnk.h read_lnk.h lnkforward.h

kde_module_LTLIBRARIES = kfile_lnk.la

kfile_lnk_la_SOURCES = kfile_lnk.cpp read_lnk.cpp
kfile_lnk_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kfile_lnk_la_LIBADD = $(LIB_KIO)
kfile_lnk_la_METASOURCES = kfile_lnk.moc

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

kde_services_DATA = kfile_lnk.desktop

mime_DATA = x-win-lnk.desktop
mimedir = $(kde_mimedir)/application

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kfile_lnk.pot
