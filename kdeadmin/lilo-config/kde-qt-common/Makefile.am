AM_CXXFLAGS = -DUSE_KDE
INCLUDES= -I$(srcdir)/../common -I$(srcdir)/.. -I. $(all_includes)

noinst_LTLIBRARIES=libwidgets.la

libwidgets_la_SOURCES = \
	mainwidget.cpp general.cpp images.cpp expert.cpp \
	EditWidget.cpp
libwidgets_la_LIBADD = $(LIB_QT) ../common/libcommon.la
libwidgets_la_METASOURCES = AUTO

noinst_HEADERS = \
	mainwidget.h ui.h general.h images.h expert.h \
	EditWidget.h 
