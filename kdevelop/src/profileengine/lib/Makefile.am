INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/lib/external_interfaces \
	-I$(top_srcdir)/lib/interfaces -I$(top_srcdir)/lib/util -I$(top_srcdir)/lib/widgets \
	-I$(top_srcdir)/lib/structure $(all_includes)
METASOURCES = AUTO
lib_LTLIBRARIES = libprofileengine.la
libprofileengine_la_LDFLAGS = $(all_libraries)
libprofileengine_la_LIBADD = $(LIB_QT) $(LIB_KDECORE) $(LIB_KIO)
libprofileengine_la_SOURCES = profileengine.cpp profile.cpp

profileincludedir = $(includedir)/kdevelop/shell/profileengine
profileinclude_HEADERS = profileengine.h profile.h

DOXYGEN_REFERENCES = dcop interfaces kdecore kdefx kdeui khtml kmdi kio kjs kparts kutils kdevutil kdevinterfaces
DOXYGEN_PROJECTNAME = KDevelop Shell Profiles Library
DOXYGEN_DOCDIRPREFIX = kdevprofiles
include ../../../Doxyfile.am
