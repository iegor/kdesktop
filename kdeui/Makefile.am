
#	This file is part of the KDE libraries
#    Copyright (C) 1997 Matthias Kalle Dalheimer (kalle@kde.org)
#			  (C) 1997 Stephan Kulow (coolo@kde.org)

#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Library General Public
#    License as published by the Free Software Foundation; either
#    version 2 of the License, or (at your option) any later version.

#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Library General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this library; see the file COPYING.  If not, write to
#    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
#    Boston, MA 02110-1301, USA.

SUBDIRS = . kdetrayproxy tests about

INCLUDES= -I$(top_srcdir)/kdefx -I$(top_srcdir)/interfaces $(all_includes)

# For the future: examine if condensing the tons of *_LDFLAGS variables
# into $(all_libraries) isn't better
AM_LDFLAGS = $(LDFLAGS_AS_NEEDED) $(LDFLAGS_NEW_DTAGS)

lib_LTLIBRARIES = libkdeui.la libkspell.la
libkdeui_la_LDFLAGS = $(KDE_MT_LDFLAGS) -no-undefined -version-info 6:0:2
libkdeui_la_LIBADD = ../kdecore/libkdecore.la

libkspell_la_LDFLAGS = $(KDE_MT_LDFLAGS) -version-info 6:0:2 -no-undefined
libkspell_la_LIBADD = libkdeui.la
libkspell_la_SOURCES = dummy.cpp

picsdir = $(kde_datadir)/kdeui/pics
pics_DATA = aboutkde.png ktip-bulb.png ktip-background.png

include_HEADERS = kprogress.h kcolordlg.h \
		kcolordialog.h kselect.h \
		kdatepik.h kdatepicker.h kdatetbl.h kfontdialog.h kpopupmenu.h \
		kfontrequester.h ktabctl.h kstatusbar.h \
		kmainwindow.h kmainwindowiface.h ktoolbar.h kmenubar.h \
		knuminput.h kseparator.h klineedit.h \
		krestrictedline.h kcolorbutton.h kcolorbtn.h \
		ksystemtray.h kdockwindow.h kbuttonbox.h \
		keditcl.h kled.h keditlistbox.h \
		kwizard.h kkeydialog.h kkeybutton.h \
		kurllabel.h kruler.h kcursor.h \
		klineeditdlg.h kinputdialog.h kactivelabel.h \
		kcharselect.h kcolordrag.h qxembed.h \
		knumvalidator.h kdialog.h kdialogbase.h \
		kjanuswidget.h kaboutdialog.h \
		kauthicon.h kmessagebox.h ksharedpixmap.h \
		kdualcolorbtn.h kdualcolorbutton.h ktoolbarbutton.h \
		ktoolbarradiogroup.h ktextbrowser.h \
		kaction.h kactioncollection.h kactionclasses.h khelpmenu.h kswitchlanguagedialog.h \
		kcmodule.h kcmenumngr.h kpanelmenu.h kpanelappmenu.h \
		kactionshortcutlist.h kstdaction.h kcombobox.h \
		kiconview.h klistview.h klistbox.h kbugreport.h kpassdlg.h \
		kxmlguiclient.h kxmlgui.h kxmlguibuilder.h kxmlguifactory.h \
		kpixmapio.h kwordwrap.h \
		kedittoolbar.h kdockwidget.h kanimwidget.h \
		krootpixmap.h kaboutkde.h kaboutapplication.h \
		kpanelapplet.h kpushbutton.h kdcopactionproxy.h kcolorcombo.h \
		kpanelextension.h kcompletionbox.h ksqueezedtextlabel.h \
		kcommand.h kwindowlistmenu.h kfontcombo.h kaccelgen.h ktip.h \
		kdatewidget.h karrowbutton.h kguiitem.h kstdguiitem.h \
		kstringvalidator.h kwindowinfo.h \
		ktextedit.h kpassivepopup.h \
		kdatetimewidget.h ktimewidget.h ktimezonewidget.h kscrollview.h \
		kspell.h ksconfig.h kspelldlg.h ksyntaxhighlighter.h \
		ktabbar.h ktabwidget.h ksplashscreen.h kconfigdialog.h \
		kactionselector.h klanguagebutton.h klistviewsearchline.h \
		kpixmapregionselectorwidget.h kpixmapregionselectordialog.h \
		kiconviewsearchline.h ktoolbarlabelaction.h kshortcutdialog.h

# the order of the sources isn't randomly (at least not completely).
# the order is predictated by the use of X11 header files
# the files on the first line need to occur first when compiling
# with --enable-final since they use X defines that get undefined
# in ksharedpixmap.cpp
libkdeui_la_SOURCES = kaboutapplication.cpp kaboutdialog.cpp kaboutkde.cpp \
	kaction.cpp kactionclasses.cpp kactioncollection.cpp kactionselector.cpp \
	kactivelabel.cpp kanimwidget.cpp karrowbutton.cpp kauthicon.cpp kbugreport.cpp \
	kbuttonbox.cpp kcharselect.cpp kcmenumngr.cpp kcmodule.cpp kcolorbutton.cpp \
	kcolorcombo.cpp kcolordialog.cpp kcolordrag.cpp kcombobox.cpp kcommand.cpp \
	kcompletionbox.cpp kconfigdialog.cpp kcursor.cpp kdatepicker.cpp kdatetbl.cpp \
	kdatetimewidget.cpp kdatewidget.cpp kdcopactionproxy.cpp kdialog.cpp kdialogbase.cpp \
	kdockwidget.cpp kdockwidget_private.cpp kdualcolorbutton.cpp keditcl1.cpp keditcl2.cpp \
	keditlistbox.cpp kedittoolbar.cpp kfontcombo.cpp kfontdialog.cpp kfontrequester.cpp \
	kguiitem.cpp khelpmenu.cpp kiconview.cpp kiconviewsearchline.cpp kinputdialog.cpp \
	kjanuswidget.cpp kkeybutton.cpp kkeydialog.cpp klanguagebutton.cpp kled.cpp \
	klineedit.cpp klineeditdlg.cpp klistbox.cpp klistview.cpp klistviewsearchline.cpp \
	kmainwindow.cpp kmainwindowiface.cpp kmainwindowiface_skel.cpp kmenubar.cpp \
	kmessagebox.cpp knuminput.cpp knumvalidator.cpp kpanelapplet.cpp kpanelappmenu.cpp \
	kpanelextension.cpp kpanelmenu.cpp kpassdlg.cpp kpassivepopup.cpp kpixmapio.cpp \
	kpixmapregionselectordialog.cpp kpixmapregionselectorwidget.cpp kpopupmenu.cpp kprogress.cpp \
	kpushbutton.cpp krestrictedline.cpp krootpixmap.cpp kruler.cpp ksconfig.cpp \
	kscrollview.cpp kselect.cpp kseparator.cpp ksharedpixmap.cpp kshortcutdialog.cpp \
	kshortcutdialog_advanced.cpp kshortcutdialog_advanced.ui kshortcutdialog_simple.cpp \
	kshortcutdialog_simple.ui kspell.cpp kspelldlg.cpp kspellui.cpp kspellui.ui ksplashscreen.cpp \
	ksqueezedtextlabel.cpp kstatusbar.cpp kstdaction.cpp kstdguiitem.cpp kstringvalidator.cpp \
	kswitchlanguagedialog.cpp ksyntaxhighlighter.cpp ksystemtray.cpp ktabbar.cpp ktabctl.cpp \
	ktabwidget.cpp ktextbrowser.cpp ktextedit.cpp ktimewidget.cpp ktimezonewidget.cpp \
	ktip.cpp ktoolbar.cpp ktoolbarbutton.cpp ktoolbarhandler.cpp \
	ktoolbarlabelaction.cpp ktoolbarradiogroup.cpp kurllabel.cpp kwhatsthismanager.cpp \
	kwindowinfo.cpp kwindowlistmenu.cpp kwizard.cpp kwordwrap.cpp kxmlguibuilder.cpp \
	kxmlguiclient.cpp kxmlguifactory.cpp kxmlguifactory_p.cpp qxembed.cpp

noinst_HEADERS = kdockwidget_private.h kdockwidget_p.h klistviewlineedit.h \
		kdialogbase_priv.h kaboutdialog_private.h kcursor_private.h \
		kdepackages.h kxmlguifactory_p.h \
		kstdaction_p.h kwhatsthismanager_p.h

scripts_SCRIPTS = preparetips
scriptsdir = $(kde_bindir)

libkdeui_la_METASOURCES = AUTO
libkdeui_la_NMCHECK = $(srcdir)/libkdeui.nmcheck
libkdeui_la_NMCHECKWEAK = $(srcdir)/libkdeui_weak.nmcheck $(top_srcdir)/kdecore/libkdecore_weak.nmcheck \
	$(top_srcdir)/dcop/libDCOP_weak.nmcheck $(top_srcdir)/kdecore/libqt-mt_weak.nmcheck

rcdir = $(kde_confdir)/ui
rc_DATA = ui_standards.rc

EXTRA_DIST = CHANGES.kdatepicker USERS.kdatepicker kpixmapio-perf.png dcolorreset.xpm kpartgui.dtd dcolorarrow.xbm colors Mainpage.dox

.PHONY: kdepackages

kdepackages:
	-rm $(srcdir)/kdepackages.h
	curl "http://bugsold.kde.org/xml.cgi?data=versiontable" > tmp_download
	echo "// DO NOT EDIT - EDIT bugs/Maintainers instead" >  $(srcdir)/kdepackages.h
	echo "const char * const packages[] = {" >> $(srcdir)/kdepackages.h
	cat tmp_download | egrep "product|component" | grep -v "</product>" | \
	sed -e "s,<product name=\"\([^\"]*\)\">,product=\"\1\"; component=\"general\",; \
	s#<component>\([^<]*\)</component>#component=\"\1\"#"  > tmp_download2
	(while read i; do eval $$i; echo "\"$$product/$$component\","; done) < tmp_download2 | \
	sed -e "s,/general\",/00general\"," | LC_ALL=C sort -u | \
	grep -v 'valgrind/' | sed -e "s,/00general\",\"," >> $(srcdir)/kdepackages.h
	echo "0 };" >>  $(srcdir)/kdepackages.h
	rm tmp_download2 tmp_download

dummy.cpp:
	echo "#ifdef _AIX" >dummy.cpp
	echo "namespace {" >> dummy.cpp
	echo "void *not_empty_file;" >>dummy.cpp
	echo "}" >> dummy.cpp
	echo "#endif" >> dummy.cpp

DOXYGEN_REFERENCES = kdecore kdefx dcop kio kutils
DOXYGEN_SET_INTERNAL_DOCS = YES
include ../admin/Doxyfile.am
