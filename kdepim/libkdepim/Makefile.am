SUBDIRS = cfgc interfaces tests icons pics kpartsdesignerplugin about

INCLUDES = -I$(top_srcdir) $(all_includes)

lib_LTLIBRARIES   = libkdepim.la
libkdepim_la_SOURCES = \
  ktimeedit.cpp \
  alarmclient.cpp \
  kprefsdialog.cpp kpimprefs.cpp \
  categoryselectdialog_base.ui categoryselectdialog.cpp \
  categoryeditdialog_base.ui categoryeditdialog.cpp \
  kdateedit.cpp kimportdialog.cpp kvcarddrag.cpp \
  sidebarextension.cpp infoextension.cpp \
  addressesdialog.cpp addresspicker.ui addresseeview.cpp \
  maillistdrag.cpp MailTransportServiceIface.skel \
  AddressBookServiceIface.skel \
  kconfigpropagator.cpp kconfigwizard.cpp \
  weaver.cpp weaverextensions.cpp weaverlogger.cpp \
  pluginloaderbase.cpp addresseelineedit.cpp addresseelineedit.skel \
  kdatepickerpopup.cpp kfileio.cpp \
  clicklineedit.cpp configmanager.cpp \
  collectingprocess.cpp ldapclient.cpp \
  overlaywidget.cpp progressmanager.cpp progressdialog.cpp \
  statusbarprogresswidget.cpp ssllabel.cpp completionordereditor.cpp \
  resourceabc.cpp diffalgo.cpp addresseediffalgo.cpp calendardiffalgo.cpp \
  htmldiffalgodisplay.cpp ldapsearchdialog.cpp broadcaststatus.cpp kresourceprefs.cpp \
  kpixmapregionselectorwidget.cpp kpixmapregionselectordialog.cpp \
  kabcresourcecached.cpp kxface.cpp \
  kaccount.cpp kaddrbook.cpp kfoldertree.cpp kregexp3.cpp \
  kscoring.cpp kscoringeditor.cpp ksubscription.cpp kwidgetlister.cpp \
  linklocator.cpp qutf7codec.cpp \
  recentaddresses.cpp spellingfilter.cpp \
  addresseeselector.cpp addresseeemailselection.cpp \
  designerfields.cpp kcmdesignerfields.cpp \
  embeddedurlpage.cpp kincidencechooser.cpp \
  groupwarejob.cpp pimemoticons.kcfgc \
  krsqueezedtextlabel.cpp csshelper.cpp distributionlist.cpp \
  kpimurlrequesterdlg.cpp sendsmsdialog.cpp kmailcompletion.cpp

MailTransportServiceIface_DCOPIDLNG = true
MailTransportServiceIface_DIR = $(srcdir)/interfaces

AddressBookServiceIface_DIR = $(srcdir)/interfaces

libkdepim_la_LDFLAGS = $(all_libraries) -no-undefined -version-info 1:0:0
libkdepim_la_LIBADD  = $(top_builddir)/libkcal/libkcal.la \
                       $(top_builddir)/libemailfunctions/libemailfunctions.la \
                       $(LIB_KIO) $(LIB_KABC) $(LIB_KPARTS) $(LIB_KIMPROXY) $(LIB_POLL) -lqui
#               $top_builddir)/libkdepim/resources/libkpimresources.la

include_HEADERS = kdepimmacros.h kpimprefs.h

AM_CXXFLAGS = -DQT_PLUGIN
kde_widget_LTLIBRARIES = kdepimwidgets.la
kdepimwidgets_la_LDFLAGS = $(KDE_PLUGIN) -module $(all_libraries)
kdepimwidgets_la_LIBADD = $(LIB_KIO) libkdepim.la
kdepimwidgets_la_SOURCES = kdepimwidgets.cpp 

kdepimwidgets.cpp: $(srcdir)/kdepim.widgets
	$(MAKEKDEWIDGETS) -o kdepimwidgets.cpp $(srcdir)/kdepim.widgets

CLEANFILES = kdepimwidgets.cpp

kde_kcfg_DATA = pimemoticons.kcfg

METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) ../libkpimidentities/*.cpp ../libemailfunctions/*.cpp *.cpp -o $(podir)/libkdepim.pot

include $(top_srcdir)/admin/Doxyfile.am
