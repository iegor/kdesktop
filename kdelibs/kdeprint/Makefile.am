AM_CPPFLAGS = -D_KDEPRINT_COMPILE

if include_cups_SUBDIR
CUPSSUBDIR=cups
endif

SUBDIRS = . management lpdunix $(CUPSSUBDIR) lpr rlpr ext filters pics tools tests

INCLUDES= -I$(top_srcdir)/kdefx -I$(top_srcdir)/kded -I$(top_srcdir) $(all_includes)

kde_module_LTLIBRARIES = kded_kdeprintd.la

kded_kdeprintd_la_SOURCES = kdeprintd.skel kdeprintd.cpp kprintprocess.cpp
kded_kdeprintd_la_LIBADD = $(LIB_KIO) $(LIB_KDED)
kded_kdeprintd_la_LDFLAGS = $(all_libraries) -module -avoid-version
#kded_kdeprintd_la_METASOURCES = AUTO

lib_LTLIBRARIES = libkdeprint.la

libkdeprint_la_SOURCES = kmfactory.cpp kmjob.cpp kmjobmanager.cpp kmmanager.cpp kmprinter.cpp \
		         kmuimanager.cpp kmvirtualmanager.cpp driver.cpp driveritem.cpp droptionview.cpp \
			 driverview.cpp kmdbentry.cpp \
		         kprintpreview.cpp kprintdialog.cpp kprinter.cpp kprinterimpl.cpp kprintdialogpage.cpp \
			 kpcopiespage.cpp kpdriverpage.cpp kprinterpropertydialog.cpp kpgeneralpage.cpp kpqtpage.cpp \
			 kmthreadjob.cpp kpipeprocess.cpp kpfilterpage.cpp \
			 kmspecialmanager.cpp kdeprintcheck.cpp plugincombobox.cpp kpreloadobject.cpp kfilelist.cpp \
			 kprintaction.cpp kpfileselectpage.cpp treecombobox.cpp kmfactory.skel kxmlcommand.cpp \
			 util.cpp printerfilter.cpp marginpreview.cpp marginvaluewidget.cpp marginwidget.cpp \
			 kpmarginpage.cpp messagewindow.cpp posterpreview.cpp kpposterpage.cpp foomatic2loader.cpp \
			 fooparser.cpp fooscanner.cpp ppdparser.cpp ppdscanner.cpp ppdloader.cpp

libkdeprint_la_LDFLAGS = $(all_libraries) -version-info 6:0:2 -no-undefined
libkdeprint_la_LIBADD = $(LIB_KPARTS) $(LIBZ)
libkdeprint_la_DEPENDENCIES =  $(LIB_KPARTS) 
libkdeprint_la_METASOURCES = AUTO

noinst_HEADERS = kmdbentry.h kmfactory.h \
		 kmuimanager.h kmvirtualmanager.h driveritem.h droptionview.h \
		 driverview.h kprintpreview.h kprinterimpl.h kprintdialog.h \
		 kpcopiespage.h kpdriverpage.h kprinterpropertydialog.h kpgeneralpage.h \
		 kpqtpage.h kprintprocess.h kmthreadjob.h kpipeprocess.h kdeprintd.h \
		 kpfilterpage.h kmspecialmanager.h kdeprintcheck.h plugincombobox.h \
		 kfilelist.h kpfileselectpage.h treecombobox.h kxmlcommand.h \
		 util.h driverparse.h printerfilter.h marginpreview.h marginwidget.h \
		 marginvaluewidget.h kpmarginpage.h messagewindow.h \
		 posterpreview.h kpposterpage.h foomatic2loader.h ppdloader.h

include_HEADERS = kprinter.h kprintaction.h

kdeprint_HEADERS = kprintdialogpage.h kpreloadobject.h kmmanager.h kmprinter.h kmobject.h kprintdialog.h \
		   driver.h kmjobmanager.h kmjob.h
kdeprintdir = $(includedir)/kdeprint

EXTRA_DIST = fooparser.y fooscanner.l kprintpreviewui.rc

KDE_OPTIONS = nofinal

# installation of UI files
uidata_DATA = kprintpreviewui.rc
uidatadir = $(kde_confdir)/ui

# Test page
printdata_DATA = testprint.ps specials.desktop
printdatadir = $(kde_datadir)/kdeprint

# Services
servicesdir = $(kde_servicesdir)/kded
services_DATA = kdeprintd.desktop

# Config file
configdir = $(kde_confdir)
config_DATA = kdeprintrc

# parser and scanner
fooparser:
	rm -f fooparser.cpp.h
	bison -v -d -p kdeprint_foomatic2 $(srcdir)/fooparser.y -o $(srcdir)/fooparser.cpp
	if test -f "$(srcdir)/fooparser.hpp"; then mv $(srcdir)/fooparser.hpp $(srcdir)/fooparser.cpp.h; fi
	flex -o$(srcdir)/fooscanner.cpp -Pkdeprint_foomatic2 $(srcdir)/fooscanner.l
	echo "#ifndef fooparser_cpp_h" > $(srcdir)/fooparser.cpp.hh
	echo "#define fooparser_cpp_h" >> $(srcdir)/fooparser.cpp.hh
	cat $(srcdir)/fooparser.cpp.h >> $(srcdir)/fooparser.cpp.hh
	echo "#endif" >> $(srcdir)/fooparser.cpp.hh
	mv $(srcdir)/fooparser.cpp.hh $(srcdir)/fooparser.cpp.h

ppdparser:
	rm -f ppdparser.cpp.h
	bison -v -d -p kdeprint_ppd $(srcdir)/ppdparser.y -o $(srcdir)/ppdparser.cpp
	if test -f "$(srcdir)/ppdparser.hpp"; then mv $(srcdir)/ppdparser.hpp $(srcdir)/ppdparser.cpp.h; fi
	flex -o$(srcdir)/ppdscanner.cpp -Pkdeprint_ppd $(srcdir)/ppdscanner.l
	echo "#ifndef ppdparser_cpp_h" > $(srcdir)/ppdparser.cpp.hh
	echo "#define ppdparser_cpp_h" >> $(srcdir)/ppdparser.cpp.hh
	cat $(srcdir)/ppdparser.cpp.h >> $(srcdir)/ppdparser.cpp.hh
	echo "#endif" >> $(srcdir)/ppdparser.cpp.hh
	mv $(srcdir)/ppdparser.cpp.hh $(srcdir)/ppdparser.cpp.h

noinst_DATA = kdeprint

kdeprint:
	$(LN_S) $(srcdir)/management kdeprint

# convenience static library
noinst_LIBRARIES = libdriverparse.a
libdriverparse_a_SOURCES = driverparse.c

update_DATA = kdeprintrc.upd
updatedir = $(kde_datadir)/kconf_update

messages: rc.cpp
	$(EXTRACTRC) `find . -name "*.rc" -o -name "*.ui" | grep -v /cupsdconf` >> rc.cpp
	$(XGETTEXT) `find . -name "*.cpp" | grep -v /cupsdconf` -o $(podir)/kdeprint.pot

include ../admin/Doxyfile.am

