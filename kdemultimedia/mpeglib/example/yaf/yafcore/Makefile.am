#  libyafshared - Makefile.am

INCLUDES		=	-I../../include $(all_includes)

EXTRA_DIST		=	PROTOCOL.v01 yafCommand.defs yafRuntime.defs


lib_LTLIBRARIES		=	libyafcore.la

noinst_HEADERS		=	commandTableYAF.h multiReader.h \
				runtimeTableYAF.h \
				outputDecoder.h \
				commandLine.h \
				commandTable.h lineStack.h \
				parser.h


libyafcore_la_SOURCES	=	buffer.cpp \
				lineStack.cpp \
				commandLine.cpp \
				parser.cpp \
				outputInterface.cpp \
				inputInterface.cpp \
				commandTable.cpp \
				commandTableYAF.cpp \
				runtimeTableYAF.cpp \
				inputDecoder.cpp \
				outputDecoder.cpp \
				yaf_control.cpp \
				multiReader.cpp

libyafcore_la_LDFLAGS	=	$(all_libraries) $(KDE_RPATH) \
				-no-undefined -avoid-version

libyafcore_la_LIBADD	=	-lkdefakes $(THIS_LIB_LIBS)




yafcoredir		=	$(includedir)/$(THIS_LIB_NAME)/util/yaf/yafcore

yafcore_HEADERS		=	inputInterface.h outputInterface.h \
				buffer.h inputDecoder.h yaf_control.h

