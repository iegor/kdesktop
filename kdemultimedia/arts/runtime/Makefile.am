####### Runtime part of artsbuilder: this part will get dynamically loaded
# into the aRts server (or other apps) to create structures which are designed
# in artsbuilder. So to speak it's the "loader for the .arts language".

AM_CXXFLAGS = -DEXAMPLES_DIR='"$(arts_datadir)/artsbuilder/examples"'
INCLUDES= -I$(arts_includes) $(all_includes)

lib_LTLIBRARIES = libartsbuilder.la

libartsbuilder_la_SOURCES = artsbuilder.cc sequenceutils.cc \
  structurebuilder_impl.cc structures_impl.cc moduleinfo.cc \
  compatibility.cc localfactory_impl.cc artsbuilderloader_impl.cc

libartsbuilder_la_LIBADD = -lmcop -lartsflow $(LIBDL)
libartsbuilder_la_COMPILE_FIRST = artsbuilder.h
libartsbuilder_la_LDFLAGS = $(all_libraries) -L$(arts_libraries) \
  -no-undefined

artsbuilder.lo: artsbuilder.h
artsbuilder.mcopclass: artsbuilder.h
artsbuilder.mcoptype: artsbuilder.h
artsbuilder.h artsbuilder.cc: $(srcdir)/artsbuilder.idl $(MCOPIDL)
	$(MCOPIDL) -t -I$(arts_includes) $(srcdir)/artsbuilder.idl

DISTCLEANFILES = artsbuilder.cc artsbuilder.h \
  artsbuilder.mcoptype artsbuilder.mcopclass

####### install idl files

artsincludedir = $(includedir)/arts
artsinclude_HEADERS = artsbuilder.h artsbuilder.idl

mcopclassdir = $(libdir)/mcop/Arts
mcopclass_DATA = StructureBuilder.mcopclass StructureDesc.mcopclass \
	LocalFactory.mcopclass ArtsBuilderLoader.mcopclass

mcoptypedir = $(libdir)/mcop
mcoptype_DATA = artsbuilder.mcoptype artsbuilder.mcopclass
