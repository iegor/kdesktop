## $Id: Makefile.am 219287 2003-04-09 12:32:40Z akrille $
## Makefile.am of kdemultimedia/tools/thumbnail

INCLUDES = -I$(kde_includes)/arts $(all_includes) $(XINE_CFLAGS)

AM_CFLAGS = -U__STRICT_ANSI__

METASOURCES = AUTO

kde_module_LTLIBRARIES = videothumbnail.la

videothumbnail_la_SOURCES = videocreator.cpp videoscaler.cpp
videothumbnail_la_LIBADD = $(XINE_LIBS) $(LIB_KDECORE)
videothumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) -pthread

noinst_HEADERS = videocreator.h

xineartsplugin_tools_videothumbnail_DATA = sprocket-small.png sprocket-medium.png sprocket-large.png

xineartsplugin_tools_videothumbnaildir = $(kde_datadir)/videothumbnail

services_DATA = videothumbnail.desktop

servicesdir = $(kde_servicesdir)
