SUBDIRS = . pics plugins

INCLUDES= $(all_includes)
include_HEADERS = kmultipageInterface.h

####### Files

bin_PROGRAMS = 	kviewshell
lib_LTLIBRARIES = libkmultipage.la
kde_module_LTLIBRARIES = kviewerpart.la emptymultipagepart.la
noinst_LTLIBRARIES = libifaces.la libkviewshell.la

libifaces_la_SOURCES = kviewpart_iface.cpp

kviewerpart_la_SOURCES = kviewpart.cpp pageSizeWidget_base.ui \
		pageSizeWidget.cpp pageSizeDialog.cpp pageSize.cpp sizePreview.cpp \
		zoom.cpp units.cpp kvsprefs.kcfgc \
		optionDialogGUIWidget_base.ui optionDialogAccessibilityWidget.ui simplePageSize.cpp

kviewerpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kviewerpart_la_LIBADD = ./libifaces.la $(LIB_KFILE) -lkparts

libkmultipage_la_SOURCES = kmultipage.cpp pageView.cpp marklist.cpp kmultipageInterface.skel \
		units.cpp pageSize.cpp simplePageSize.cpp renderedDocumentPage.cpp renderedDocumentPagePixmap.cpp \
		renderedDocumentPagePrinter.cpp documentPageCache.cpp documentWidget.cpp searchWidget.cpp \
		selection.cpp documentRenderer.cpp history.cpp kvsprefs.kcfgc tableOfContents.cpp \
		kprintDialogPage_pageoptions.cpp


kde_kcfg_DATA = kviewshell.kcfg

libkmultipage_la_LDFLAGS = $(all_libraries)
libkmultipage_la_LIBADD = $(LIB_KFILE) -lkdeprint -lkparts

emptymultipagepart_la_SOURCES = empty_multipage.cpp emptyRenderer.cpp
emptymultipagepart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
emptymultipagepart_la_LIBADD = -lkparts libkmultipage.la

libkviewshell_la_SOURCES = kviewshell.cpp

kviewshell_SOURCES = main.cpp
kviewshell_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kviewshell_LDADD = libkviewshell.la ./libifaces.la -lkparts

kviewshellinclude_HEADERS = anchor.h bookmark.h pageView.h documentPageCache.h documentRenderer.h documentWidget.h \
	history.h hyperlink.h kmultipage.h length.h pageNumber.h pageSize.h renderedDocumentPage.h renderedDocumentPagePixmap.h \
	renderedDocumentPagePrinter.h selection.h simplePageSize.h textBox.h zoom.h zoomlimits.h
kviewshellincludedir = $(includedir)/kviewshell


METASOURCES = AUTO

rc_DATA = kviewshell.rc
rcdir = $(kde_datadir)/kviewshell
partrc_DATA = kviewerpart.rc
partrcdir = $(kde_datadir)/kviewerpart

servicetype_DATA = kmultipage.desktop
servicetypedir = $(kde_servicetypesdir)

kde_services_DATA = emptymultipage.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kviewshell.pot


include ../admin/Doxyfile.am
