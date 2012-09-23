INCLUDES= -I$(kde_includes)/arts $(all_includes) $(XINE_CFLAGS)

AM_CFLAGS = -U__STRICT_ANSI__

lib_LTLIBRARIES = libarts_xine.la

libarts_xine_la_SOURCES = xinePlayObject.cc \
			  xinePlayObject_impl.cpp \
			  audio_fifo_out.c
libarts_xine_la_LDFLAGS = $(all_libraries) -module -no-undefined -pthread
libarts_xine_la_LIBADD = $(XINE_LIBS) $(LIB_X11) $(LIB_XEXT) \
			 -lkmedia2_idl -lsoundserver_idl -lartsflow

libarts_xine_la_METASOURCES = AUTO

$(srcdir)/xinePlayObject_impl.cpp: xinePlayObject.h
xinePlayObject.cc xinePlayObject.h: $(srcdir)/xinePlayObject.idl
	$(MCOPIDL) -I$(kde_includes)/arts $(srcdir)/xinePlayObject.idl

noinst_HEADERS = xinePlayObject_impl.h audio_fifo_out.h

mcopclassdir = $(libdir)/mcop
mcopclass_DATA = xineAudioPlayObject.mcopclass xineVideoPlayObject.mcopclass

SUBDIRS = tools
