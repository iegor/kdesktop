
# ---- @OS_TYPE@/@ARCH_TYPE@ ----

INCLUDES                = 	$(all_includes)

EXTRA_DIST		=	audioIO_AIX.cpp audioIO_BeOS.cpp \
				audioIO_HPUX.cpp \
				audioIO_IRIX.cpp audioIO_Linux.cpp \
				audioIO_SunOS.cpp audioIO_SDL.cpp

noinst_HEADERS		=	audioIO.h dspWrapper.h

noinst_LTLIBRARIES	=	libaudio.la

libaudio_la_SOURCES	=	audioIO.cpp dspWrapper.cpp



