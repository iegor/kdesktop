#  liboutplugin - Makefile.am

INCLUDES                =       $(all_includes)


noinst_LTLIBRARIES	=	liboutput.la

noinst_HEADERS		=	windowOut.h \
				audioData.h audioDataArray.h \
				performance.h yuvDumper.h

kmpgincludedir          =       $(includedir)/$(THIS_LIB_NAME)/output

kmpginclude_HEADERS     =       outputStream.h pluginInfo.h \
				outPlugin.h dspX11OutputStream.h \
				artsOutputStream.h audioTime.h \
				avSyncer.h threadSafeOutputStream.h


liboutput_la_SOURCES	=	outPlugin.cpp outputStream.cpp \
				dspX11OutputStream.cpp \
				windowOut.cpp \
				audioTime.cpp \
				audioData.cpp audioDataArray.cpp \
				avSyncer.cpp performance.cpp \
				artsOutputStream.cpp \
				pluginInfo.cpp yuvDumper.cpp \
				threadSafeOutputStream.cpp
























