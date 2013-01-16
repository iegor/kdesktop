INCLUDES= $(all_includes)
METASOURCES = AUTO

bin_PROGRAMS = secpolicy
secpolicy_SOURCES = main.cpp secpolicywin.cpp pamview.cpp ppitem.cpp
secpolicy_LDADD = $(LIB_KIO)
secpolicy_LDFLAGS = $(all_libraries) $(KDE_RPATH)

noinst_HEADERS = secpolicywin.h pamview.h ppitem.h

EXTRA_DIST = 

messages:
	$(XGETTEXT) *.cpp -o $(podir)/secpolicy.pot
