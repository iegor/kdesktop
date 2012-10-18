INCLUDES= $(akode_includes) -I$(kde_includes)/arts $(all_includes)

noinst_HEADERS = akodePlayObject_impl.h

lib_LTLIBRARIES = libarts_akode.la

libarts_akode_la_SOURCES = akodearts.cc akodePlayObject_impl.cpp \
        akodeMPCPlayObject_impl.cpp akodeMPEGPlayObject_impl.cpp \
        akodeFFMPEGPlayObject_impl.cpp akodeXiphPlayObject_impl.cpp \
        akodeVorbisStreamPlayObject_impl.cpp akodeSpeexStreamPlayObject_impl.cpp
libarts_akode_la_LDFLAGS = $(all_libraries) -module -no-undefined
libarts_akode_la_LIBADD =  $(akode_libs) -lkmedia2_idl -lsoundserver_idl -lartsflow
libarts_akode_la_METASOURCES = AUTO
libarts_akode_la_COMPILE_FIRST = akodearts.h

$(srcdir)/akodePlayObject_impl.cpp: akodearts.h
akodearts.mcopclass: akodearts.h
akodearts.mcoptype: akodearts.h
akodearts.cc akodearts.h: $(srcdir)/akodearts.idl
	$(MCOPIDL) -t $(INCLUDES) $(srcdir)/akodearts.idl

mcoptypedir = $(libdir)/mcop
mcoptype_DATA = akodearts.mcoptype akodearts.mcopclass

if include_akode_mpeg
AKODE_MPEG=akodeMPEGPlayObject.mcopclass
endif

if include_akode_mpc
AKODE_MPC=akodeMPCPlayObject.mcopclass
endif

if include_akode_xiph
AKODE_XIPH=akodeXiphPlayObject.mcopclass akodeVorbisStreamPlayObject.mcopclass akodeSpeexStreamPlayObject.mcopclass
endif

if include_akode_ffmpeg
AKODE_FFMPEG=akodeFFMPEGPlayObject.mcopclass
endif

mcopclassdir = $(libdir)/mcop
mcopclass_DATA = akodePlayObject.mcopclass \
		 $(AKODE_MPC) $(AKODE_MPEG) $(AKODE_XIPH) $(AKODE_FFMPEG)

CLEANFILES=akodearts.h akodearts.cc akodearts.mcopclass akodearts.mcoptype

