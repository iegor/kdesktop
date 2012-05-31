#  player - Makefile.am

SUBDIRS			=	mmx abstract audio file render


INCLUDES                =       $(all_includes)



THIS_EXTRALIBS		=	abstract/libutilabstract.la \
				audio/libaudio.la \
				file/libsimplefile.la \
				render/libutilrender.la \
				mmx/libmmx.la


noinst_LTLIBRARIES      =	libutil.la

noinst_HEADERS		=	syncClockMPEG.h

kmpgincludedir          =       $(includedir)/$(THIS_LIB_NAME)/util

kmpginclude_HEADERS     =       timeStamp.h dynBuffer.h \
				timeStampArray.h syncClock.h timeWrapper.h



libutil_la_SOURCES	=	timeStamp.cpp \
				timeStampArray.cpp \
				dynBuffer.cpp syncClock.cpp \
				syncClockMPEG.cpp timeWrapper.cpp

libutil_la_LIBADD	=	$(THIS_EXTRALIBS)




























