INCLUDES = $(all_includes)

METASOURCES = AUTO

kinterfacedesignerdir = $(includedir)/kinterfacedesigner
kinterfacedesigner_HEADERS = designer.h

lib_LTLIBRARIES =  libkinterfacedesigner.la
libkinterfacedesigner_la_LIBADD = $(LIB_QT) $(LIB_KPARTS) $(LIB_KDEUI)
libkinterfacedesigner_la_LDFLAGS = $(all_libraries)
libkinterfacedesigner_la_SOURCES =  designer.cpp

DOXYGEN_REFERENCES = dcop interfaces kdecore kdefx kdeui khtml kmdi kio kjs kparts kutils
DOXYGEN_PROJECTNAME = KInterfaceDesigner Library
include ../../../Doxyfile.am
