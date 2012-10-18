#  libinputplugin - Makefile.am

EXTRA_DIST		=	cdromAccess_Linux.cpp cdromAccess_Empty.cpp \
				cdigrap.cpp README

INCLUDES                =       $(all_includes)

noinst_LTLIBRARIES	=	libinput.la

noinst_HEADERS		=	cdromToc.h cdromRawAccess.h \
				simpleRingBuffer.h fileAccessWrapper.h

kmpgincludedir          =       $(includedir)/$(THIS_LIB_NAME)/input

kmpginclude_HEADERS     =       inputStream.h fileInputStream.h \
				inputPlugin.h \
				cdromInputStream.h bufferInputStream.h \
				inputDetector.h httpInputStream.h \
				threadSafeInputStream.h cddaInputStream.h



libinput_la_SOURCES	=	inputStream.cpp fileInputStream.cpp \
				inputPlugin.cpp \
				cdromToc.cpp cdromRawAccess.cpp \
				cdromInputStream.cpp \
				bufferInputStream.cpp \
				simpleRingBuffer.cpp \
				cdromAccess.cpp inputDetector.cpp \
				httpInputStream.cpp \
				threadSafeInputStream.cpp \
				cddaInputStream.cpp \
				fileAccessWrapper.cpp

# workaround for compile errors caused by linux/cdrom.h. 
# Linux kernel headers suck donkeyballs.
KDE_CXXFLAGS = $(ENABLE_PERMISSIVE_FLAG)
