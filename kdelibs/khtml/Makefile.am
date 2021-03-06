#    This file is part of the KDE libraries
#    Copyright (C) 1997 Martin Jones (mjones@kde.org)
#              (C) 1997 Torben Weis (weis@kde.org)

#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Library General Public
#    License as published by the Free Software Foundation; either
#    version 2 of the License, or (at your option) any later version.

#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Library General Public License for more details.

#    You should have received a copy of the GNU Library General Public License
#    along with this library; see the file COPYING.LIB.  If not, write to
#    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
#    Boston, MA 02110-1301, USA.

KDE_CXXFLAGS = $(WOVERLOADED_VIRTUAL)

if DOJAVA
JAVA_SUBDIR=java
LIBKJAVA=
endif

SUBDIRS = misc dom css xml html rendering pics $(JAVA_SUBDIR) ecma . kmultipart

CLEANFILES = dummy.cpp

lib_LTLIBRARIES = libkhtml.la
kde_module_LTLIBRARIES = libkhtmlpart.la khtmlimagepart.la

libkhtml_la_SOURCES = kjserrordlg.ui khtmlview.cpp khtml_part.cpp khtml_run.cpp\
			khtml_factory.cpp khtml_settings.cc khtml_events.cpp \
			khtml_ext.cpp khtml_pagecache.cpp khtml_iface.skel \
			khtml_iface.cc khtml_printsettings.cpp htmlpageinfo.ui \
			khtmlimage.cpp

libkhtml_la_METASOURCES = AUTO

include_HEADERS = khtmlview.h khtml_part.h khtml_events.h khtml_settings.h khtmldefaults.h

noinst_HEADERS = design.h testkhtml.h test_regression.h khtml_ext.h \
		 khtml_pagecache.h khtmlimage.h khtmlpart_p.h khtml_printsettings.h \
		kjserrordlg.ui.h

if include_VERSION_SCRIPT
VSCRIPT = -Wl,--version-script=$(srcdir)/libkhtml.map
endif

libkhtml_la_LDFLAGS = -version-info 6:0:2 -no-undefined $(VSCRIPT) $(all_libraries)
libkhtml_la_LIBADD = ./xml/libkhtmlxml.la ./html/libkhtmlhtml.la \
	./rendering/libkhtmlrender.la ./css/libkhtmlcss.la \
	./misc/libkhtmlmisc.la ecma/libkjs_html.la ./dom/libkhtmldom.la $(LIBKJAVA) \
	$(LIB_KPARTS) \
	$(LIBTHAI) \
	$(top_builddir)/kdeprint/libkdeprint.la  \
	$(top_builddir)/kutils/libkutils.la \
	$(top_builddir)/kwallet/client/libkwalletclient.la

# init_libkhtml is in libkhtml.so; init_libkhtmlpart needs to be in the part
dummy.cpp: $(srcdir)/Makefile.am
	echo '#include <kdelibs_export.h>' > $@
	echo 'extern "C" KDE_EXPORT void* init_libkhtml();' >> $@
	echo 'extern "C" KDE_EXPORT void* init_libkhtmlpart() { return init_libkhtml(); } ' >> $@

libkhtmlpart_la_SOURCES = dummy.cpp
libkhtmlpart_la_LDFLAGS = -no-undefined -module $(KDE_PLUGIN) $(all_libraries)
libkhtmlpart_la_LIBADD = libkhtml.la

# init_khtmlimagefactory is in libkhtml.so; init_khtmlimagepart needs to be in the part
imgdummy.cpp: $(srcdir)/Makefile.am
	echo '#include <kdelibs_export.h>' > $@
	echo 'extern "C"  KDE_EXPORT void* init_khtmlimagefactory();' >> $@
	echo 'extern "C"  KDE_EXPORT void* init_khtmlimagepart() { return init_khtmlimagefactory(); } ' >> $@

khtmlimagepart_la_SOURCES = imgdummy.cpp
khtmlimagepart_la_LDFLAGS = -no-undefined -module $(KDE_PLUGIN) $(all_libraries)
khtmlimagepart_la_LIBADD = libkhtml.la 

INCLUDES = -I$(top_srcdir)/kjs -I$(top_builddir)/kjs -I$(top_srcdir)/kimgio \
	-I$(srcdir)/java -I$(top_srcdir)/dcop -I$(srcdir)/misc \
	-I$(srcdir)/dom -I$(srcdir)/xml -I$(srcdir)/html -I$(srcdir)/css \
	-I$(top_srcdir)/libltdl \
	-I$(top_srcdir)/kio/kssl -I$(top_builddir)/kio/kssl \
	-I$(top_srcdir)/kdeprint \
	-I$(top_srcdir)/interfaces \
	-I$(top_srcdir)/interfaces/kregexpeditor \
        -I$(top_srcdir)/kwallet/client \
	-I$(top_srcdir)/kutils \
	-I$(top_srcdir) $(all_includes)

servicedir = $(kde_servicesdir)
service_DATA = khtml.desktop khtmlimage.desktop

rcdir = $(kde_datadir)/khtml
rc_DATA = khtml.rc khtml_browser.rc khtml_popupmenu.rc

SRCDOC_DEST=$(kde_htmldir)/en/kdelibs/khtml

## generate lib documentation
srcdoc:
	$(mkinstalldirs) $(SRCDOC_DEST)
	kdoc -H -d $(SRCDOC_DEST) kdecore \
	         $(include_HEADERS) -lqt

EXTRA_DIST =  CHANGES DESIGN README.HTMLWidget README.tags 

parser:
	cd $(srcdir) && \
	perl scripts/makeattrs && \
	bash scripts/maketags && \
	bash scripts/makeprop

## test program
check_PROGRAMS 	= testkhtml
EXTRA_PROGRAMS = testkhtml_static testregression
testkhtml_SOURCES = testkhtml.cpp domtreeview.cpp
testkhtml_LDADD = libkhtml.la 
testkhtml_static_SOURCES = testkhtml.cpp domtreeview.cpp 
testkhtml_static_LDADD = libkhtml.la
testkhtml_static_LDFLAGS = -static
testregression_SOURCES = test_regression.cpp test_regression_fontoverload.cpp
testregression_LDADD = libkhtml.la

DOXYGEN_REFERENCES = kdecore kdeui kio kdefx kparts
DOXYGEN_EXCLUDE =  test*.* css ecma html java kmultipart misc pics rendering test xml

include ../admin/Doxyfile.am

.PHONY: parser

