INCLUDES= -I$(kde_includes)/arts $(all_includes)
KDE_OPTIONS = nofinal

DISTCLEANFILES		=	winskinvis.h winskinvis.cc

winskinvis.mcopclass: winskinvis.h
winskinvis.mcoptype: winskinvis.h
winskinvis.cc winskinvis.h :	$(srcdir)/winskinvis.idl
	$(MCOPIDL) -t -I$(kde_includes)/arts $(srcdir)/winskinvis.idl

lib_LTLIBRARIES		=	libwinskinvis.la
libwinskinvis_la_SOURCES=	winskinvis.cc \
				winSkinFFT_impl.cpp realFFT.cpp \
				realFFTFilter.cpp visQueue.cpp


libwinskinvis_la_LDFLAGS=	$(all_libraries) -avoid-version \
				-no-undefined

libwinskinvis_la_LIBADD =	-lkmedia2_idl -lsoundserver_idl -lartsflow
libwinskinvis_la_COMPILE_FIRST = winskinvis.cc
libwinskinvis_la_METASOURCES =	AUTO



mcoptypedir		=	$(libdir)/mcop
mcoptype_DATA		=	winskinvis.mcoptype winskinvis.mcopclass

mcopclassdir		=	$(libdir)/mcop/Noatun
mcopclass_DATA		=	WinSkinFFT.mcopclass

noatuninclude_HEADERS	=	winskinvis.h

noatunincludedir	=	$(includedir)/noatun


winSkinFFT_impl.lo: winskinvis.h
winskinvis.lo: winskinvis.h

