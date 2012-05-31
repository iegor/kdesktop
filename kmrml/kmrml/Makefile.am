SUBDIRS = server lib kcontrol
INCLUDES= -I$(top_srcdir)/kmrml/kmrml/lib $(all_includes)
METASOURCES = AUTO

LIB_KMRMLSTUFF = $(top_builddir)/kmrml/kmrml/lib/libkmrmlstuff.la

####### Files

kde_module_LTLIBRARIES = kio_mrml.la libkmrmlpart.la

kio_mrml_la_SOURCES = mrml.cpp
kio_mrml_la_LIBADD  = $(LIB_KMRMLSTUFF) $(LIB_KIO)
kio_mrml_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module

libkmrmlpart_la_SOURCES = mrml_part.cpp mrml_view.cpp loader.cpp \
	mrml_elements.cpp mrml_creator.cpp browser.cpp algorithmdialog.cpp \
	collectioncombo.cpp algorithmcombo.cpp propertysheet.cpp
libkmrmlpart_la_LIBADD = $(LIB_KMRMLSTUFF) $(LIB_KPARTS)
libkmrmlpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)

services_DATA = mrml.protocol mrml_part.desktop
servicesdir = $(kde_servicesdir)

mimetypes_DATA = mrml.desktop
mimetypesdir = $(kde_mimedir)/text

servicemenu_DATA = mrml-servicemenu.desktop
servicemenudir = $(kde_datadir)/konqueror/servicemenus

#############################################
bin_PROGRAMS = 
lib_LTLIBRARIES = 
kdeinit_LTLIBRARIES = mrmlsearch.la

mrmlsearch_la_LIBADD = $(LIB_KMRMLSTUFF) $(LIB_KDECORE)
mrmlsearch_la_LDFLAGS = $(all_libraries) -module -avoid-version
mrmlsearch_la_SOURCES = mrmlsearch.cpp

messages:
	$(EXTRACTRC) */*.ui > rc.cpp
	$(XGETTEXT) *.h *.cpp */*.cpp */*.h -o $(podir)/kmrml.pot
