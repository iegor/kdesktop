
INCLUDES = $(LIBFREETYPE_CFLAGS) $(all_includes)

SUBDIRS = pics examples

KDE_OPTIONS = nofinal

####
# common lib

lib_LTLIBRARIES = libkpovmodeler.la
libkpovmodeler_la_SOURCES = pmpart.cpp pmfactory.cpp \
	pmview.cpp pmshell.cpp pmobjectdrag.cpp \
	pmtreeview.cpp pmmessage.cpp pmtreeviewitem.cpp \
	pmerrordialog.cpp pminsertpopup.cpp pminserterrordialog.cpp \
	pmglview.cpp pmrendermanager.cpp pmobjectselect.cpp \
	pmrendermodesdialog.cpp pmpovrayrenderwidget.cpp pmpovraywidget.cpp \
	pmpovrayoutputwidget.cpp pmsettingsdialog.cpp \
	pmcolorsettings.cpp pmgridsettings.cpp pmlayoutsettings.cpp \
	pmobjectlibrarysettings.cpp pmobjectsettings.cpp pmpluginsettings.cpp \
	pmpovraysettings.cpp pmpreviewsettings.cpp pmopenglsettings.cpp \
	pmdockwidget.cpp pmdockwidget_private.cpp pmviewlayoutmanager.cpp \
	pmviewbase.cpp pmviewfactory.cpp pmunknownview.cpp pmdragwidget.cpp \
	\
	pmprototypemanager.cpp \
	pmobject.cpp pmcompositeobject.cpp \
	pmgraphicalobject.cpp pmsolidobject.cpp \
	pmscene.cpp pmglobalsettings.cpp pmskysphere.cpp pmrainbow.cpp \
	pmfog.cpp pmbox.cpp pmsphere.cpp pmblob.cpp pmblobsphere.cpp \
	pmblobcylinder.cpp pmtext.cpp pmjuliafractal.cpp \
	pmcylinder.cpp pmcone.cpp pmtorus.cpp pmplane.cpp pmpolynom.cpp \
	pmdisc.cpp pmbicubicpatch.cpp pmtriangle.cpp \
	pmlathe.cpp pmprism.cpp pmsor.cpp pmsqe.cpp pmheightfield.cpp \
	pmheightfieldroam.cpp pmcomment.cpp pmraw.cpp pmnamedobject.cpp \
	pmtranslate.cpp pmscale.cpp pmrotate.cpp pmpovraymatrix.cpp \
	pmcsg.cpp pmcamera.cpp pmboundedby.cpp pmclippedby.cpp \
	pmlight.cpp pmlookslike.cpp pmprojectedthrough.cpp\
	pmtexturebase.cpp pmtexture.cpp pmpigment.cpp \
	pmsolidcolor.cpp pmlistpattern.cpp pmquickcolor.cpp \
	pmpattern.cpp pmblendmapmodifiers.cpp pmtexturemap.cpp \
	pmnormal.cpp pmslope.cpp pmwarp.cpp pmfinish.cpp \
	pminterior.cpp pmmedia.cpp pmmaterial.cpp pmmaterialmap.cpp \
	pmdensity.cpp pmimagemap.cpp pmbumpmap.cpp \
	\
	pmisosurface.cpp pmradiosity.cpp pmglobalphotons.cpp \
	pmphotons.cpp pmlightgroup.cpp pminteriortexture.cpp \
	pmspheresweep.cpp pmmesh.cpp pmdetailobject.cpp \
	\
	pmdeclare.cpp pmobjectlink.cpp \
	pmrecursiveobjectiterator.cpp \
	\
	pmaddcommand.cpp pmcommandmanager.cpp pmdatachangecommand.cpp \
	pmdeletecommand.cpp pmmovecommand.cpp \
	\
	pmdialogview.cpp pmdialogeditbase.cpp pmgraphicalobjectedit.cpp \
	pmnamedobjectedit.cpp pmsolidobjectedit.cpp pmskysphereedit.cpp \
	pmglobalsettingsedit.cpp pmrainbowedit.cpp pmfogedit.cpp \
	pmboxedit.cpp pmsphereedit.cpp pmblobedit.cpp pmblobsphereedit.cpp \
	pmblobcylinderedit.cpp pmtextedit.cpp pmjuliafractaledit.cpp \
	pmcylinderedit.cpp pmconeedit.cpp pmtorusedit.cpp pmplaneedit.cpp \
	pmpolynomedit.cpp pmheightfieldedit.cpp \
	pmlatheedit.cpp pmprismedit.cpp pmsoredit.cpp pmsqeedit.cpp \
	pmdiscedit.cpp pmbicubicpatchedit.cpp pmtriangleedit.cpp \
	pmcommentedit.cpp pmrawedit.cpp \
	pmrotateedit.cpp pmscaleedit.cpp pmtranslateedit.cpp \
	pmpovraymatrixedit.cpp \
	pmcsgedit.cpp pmcameraedit.cpp pmlightedit.cpp pmboundedbyedit.cpp \
	pmclippedbyedit.cpp \
	pmlineedits.cpp pmvectorlistedit.cpp pmcoloredit.cpp pmlinkedit.cpp \
	pmvectoredit.cpp pmpalettevalueedit.cpp pmformulalabel.cpp \
	pmtexturebaseedit.cpp pmtextureedit.cpp pmpigmentedit.cpp \
	pmsolidcoloredit.cpp pmlistpatternedit.cpp pmquickcoloredit.cpp \
	pmpatternedit.cpp pmblendmapmodifiersedit.cpp pmimagemapedit.cpp \
	pmtexturemapedit.cpp pmbumpmapedit.cpp pmmaterialmapedit.cpp \
	pmnormaledit.cpp pmslopeedit.cpp pmfinishedit.cpp pmwarpedit.cpp \
	pminterioredit.cpp pmmediaedit.cpp pmmaterialedit.cpp \
	pmdensityedit.cpp pmdeclareedit.cpp pmobjectlinkedit.cpp \
	\
	pmisosurfaceedit.cpp pmradiosityedit.cpp pmglobalphotonsedit.cpp \
	pmphotonsedit.cpp pmlightgroupedit.cpp pminteriortextureedit.cpp \
	pmspheresweepedit.cpp pmmeshedit.cpp pmdetailobjectedit.cpp \
	\
	pmcontrolpoint.cpp pm3dcontrolpoint.cpp pm2dcontrolpoint.cpp \
	pmsorcontrolpoint.cpp pmtranslatecontrolpoint.cpp \
	pmrotatecontrolpoint.cpp pmscalecontrolpoint.cpp \
	pmvectorcontrolpoint.cpp pmdistancecontrolpoint.cpp \
	pmplanenormalcontrolpoint.cpp \
	\
	pmmemento.cpp pmmapmemento.cpp pmsplinememento.cpp pmprismmemento.cpp \
	pmpalettevaluememento.cpp \
	\
	pmiomanager.cpp pmpovrayformat.cpp pmpovray31format.cpp pmpovray35format.cpp \
	pmpovray31serialization.cpp pmpovray35serialization.cpp \
	pmscanner.cpp pmparser.cpp pmxmlparser.cpp pmpovrayparser.cpp \
	pmserializer.cpp pmoutputdevice.cpp pmxmlhelper.cpp \
	pmfiledialog.cpp \
	\
	pmpalettevalue.cpp \
	pmvector.cpp pmmath.cpp pmmatrix.cpp pmviewstructure.cpp pmline.cpp \
	pmcolor.cpp pmpoint.cpp pmsymboltable.cpp pmactions.cpp \
	pmsplinesegment.cpp pmsorsegment.cpp pmpolynomexponents.cpp \
	pmvariant.cpp pmmetaobject.cpp pmenumproperty.cpp pmface.cpp \
	\
	pmrendermode.cpp pmresourcelocator.cpp pmtruetypecache.cpp \
	pmdocumentationmap.cpp pminsertrulesystem.cpp pmlibrarymanager.cpp \
	pmlibraryhandle.cpp pmlibraryhandleedit.cpp pmlibraryobject.cpp \
	pmlibraryentrypreview.cpp\
	pmlibrarybrowser.cpp pmlibraryiconview.cpp pmlibraryobjectsearch.cpp \
	pmpluginmanager.cpp \
	pmpartiface.skel

libkpovmodeler_la_LDFLAGS = $(all_libraries) -version-info 0:0:0 -no-undefined $(LIBFREETYPE_RPATH)
libkpovmodeler_la_LIBADD = $(LIBFREETYPE_LIBS) $(LIB_KPARTS) $(GLLIB) -lXmu -lXi
METASOURCES = AUTO

noinst_HEADERS = pmpart.h pmfactory.h pmview.h pmshell.h pmobjectdrag.h \
	pmtreeview.h pmtreeviewitem.h pmmessage.h \
	pmerrordialog.h pminsertpopup.h pminserterrordialog.h \
	pmglview.h pmrendermanager.h pmobjectselect.h \
	pmrendermodesdialog.h pmpovrayrenderwidget.h pmpovraywidget.h \
	pmpovrayoutputwidget.h pmsettingsdialog.h \
	pmcolorsettings.h pmgridsettings.h pmlayoutsettings.h \
	pmobjectlibrarysettings.h pmobjectsettings.h pmpluginsettings.h \
	pmpovraysettings.h pmpreviewsettings.h pmopenglsettings.h \
	pmdockwidget.h pmdockwidget_private.h pmviewlayoutmanager.h \
	pmviewbase.h pmviewfactory.h pmunknownview.h pmdragwidget.h \
	\
	pmprototypemanager.h pmallobjects.h \
	pmobject.h pmcompositeobject.h \
	pmgraphicalobject.h pmsolidobject.h \
	pmscene.h pmglobalsettings.h pmskysphere.h pmrainbow.h \
	pmfog.h pmbox.h pmsphere.h pmblob.h pmblobsphere.h \
	pmblobcylinder.h pmtext.h pmjuliafractal.h \
	pmcylinder.h pmcone.h pmtorus.h pmplane.h pmpolynom.h \
	pmdisc.h pmbicubicpatch.h pmtriangle.h \
	pmlathe.h pmprism.h pmsor.h pmsqe.h pmheightfield.h \
	pmheightfieldroam.h pmcomment.h pmraw.h pmnamedobject.h \
	pmtranslate.h pmscale.h pmrotate.h pmpovraymatrix.h \
	pmcsg.h pmcamera.h pmboundedby.h pmclippedby.h \
	pmlight.h pmlookslike.h pmprojectedthrough.h\
	pmtexturebase.h pmtexture.h pmpigment.h \
	pmsolidcolor.h pmlistpattern.h pmquickcolor.h pmmaterialmap.h \
	pmpattern.h pmblendmapmodifiers.h pmtexturemap.h pmnormal.h \
	pmslope.h pmfinish.h pmwarp.h pminterior.h pmmedia.h pmmaterial.h \
	pmdensity.h pmimagemap.h pmbumpmap.h pmdeclare.h pmobjectlink.h \
	pmrecursiveobjectiterator.h \
	\
	pmisosurface.h pmradiosity.h pmglobalphotons.h \
	pmphotons.h pmlightgroup.h pminteriortexture.h \
	pmspheresweep.h pmmesh.h pmdetailobject.h \
	\
	pmcommand.h pmaddcommand.h pmcommandmanager.h pmdatachangecommand.h \
	pmallcommands.h pmdeletecommand.h pmmovecommand.h \
	\
	pmdialogview.h pmdialogeditbase.h pmgraphicalobjectedit.h \
	pmnamedobjectedit.h pmsolidobjectedit.h pmskysphereedit.h \
	pmglobalsettingsedit.h pmrainbowedit.h pmfogedit.h \
	pmboxedit.h pmsphereedit.h pmblobedit.h pmblobsphereedit.h \
	pmblobcylinderedit.h pmtextedit.h pmjuliafractaledit.h \
	pmcylinderedit.h pmconeedit.h pmtorusedit.h pmplaneedit.h \
	pmpolynomedit.h pmheightfieldedit.h \
	pmdiscedit.h pmbicubicpatchedit.h pmtriangleedit.h \
	pmlatheedit.h pmprismedit.h pmsoredit.h pmsqeedit.h \
	pmcommentedit.h pmrawedit.h \
	pmrotateedit.h pmscaleedit.h pmtranslateedit.h \
	pmpovraymatrixedit.h \
	pmcsgedit.h pmcameraedit.h pmlightedit.h pmboundedbyedit.h \
	pmclippedbyedit.h \
	pmlineedits.h pmvectorlistedit.h pmcoloredit.h pmlinkedit.h \
	pmvectoredit.h pmpalettevalueedit.h pmformulalabel.h \
	pmtexturebaseedit.h pmtextureedit.h pmpigmentedit.h \
	pmsolidcoloredit.h pmlistpatternedit.h pmquickcoloredit.h \
	pmpatternedit.h pmblendmapmodifiersedit.h pmimagemapedit.h \
	pmtexturemapedit.h pmbumpmapedit.h pmmaterialmapedit.h \
	pmnormaledit.h pmslopeedit.h pmfinishedit.h pmwarpedit.h \
	pminterioredit.h pmmediaedit.h pmmaterialedit.h pmdensityedit.h \
	pmdeclareedit.h pmobjectlinkedit.h \
	\
	pmisosurfaceedit.h pmradiosityedit.h pmglobalphotonsedit.h \
	pmphotonsedit.h pmlightgroupedit.h pminteriortextureedit.h \
	pmspheresweepedit.h pmmeshedit.h pmdetailobjectedit.h \
	\
	pmcontrolpoint.h pm3dcontrolpoint.h pm2dcontrolpoint.h \
	pmsorcontrolpoint.h pmtranslatecontrolpoint.h \
	pmrotatecontrolpoint.h pmscalecontrolpoint.h \
	pmvectorcontrolpoint.h pmdistancecontrolpoint.h \
	pmplanenormalcontrolpoint.h \
	\
	pmmemento.h pmmapmemento.h pmsplinememento.h pmprismmemento.h \
	pmpalettevaluememento.h \
	\
	pmiomanager.h pmpovrayformat.h pmpovray31format.h pmpovray35format.h \
	pmpovray31serialization.h pmpovray35serialization.h \
	pmtokens.h pmscanner.h \
	pmparser.h pmxmlparser.h pmpovrayparser.h \
	pmserializer.h pmoutputdevice.h pmxmlhelper.h \
	pmfiledialog.h \
	\
	pmerrorflags.h pmpalettevalue.h \
	pmvector.h pmmath.h pmmatrix.h pmviewstructure.h pmline.h \
	pmcolor.h pmpoint.h pmvalue.h pmsymboltable.h pmactions.h \
	pmobjectaction.h pmsplinesegment.h pmsorsegment.h pmpolynomexponents.h \
	pmface.h \
	\
	pmrendermode.h pmglobals.h pmdefaults.h pmresourcelocator.h \
	pmtruetypecache.h pmdocumentationmap.h pminsertrulesystem.h \
	pmlibrarymanager.h pmlibraryhandle.h pmlibraryhandleedit.h \
	pmlibraryobject.h pmlibrarybrowser.h pmlibraryiconview.h \
	pmlibraryentrypreview.h\
	pmlibraryobjectsearch.h \
	pmpluginmanager.h \
	\
	version.h pmdocumentformat.h pmdebug.h pmpartiface.h

####
# the part

kde_module_LTLIBRARIES = libkpovmodelerpart.la

libkpovmodelerpart_la_SOURCES = dummy.cpp
libkpovmodelerpart_la_LIBADD = libkpovmodeler.la
libkpovmodelerpart_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

####
# the executable

bin_PROGRAMS = kpovmodeler

kpovmodeler_SOURCES = main.cpp
kpovmodeler_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kpovmodeler_LDADD = libkpovmodeler.la

dummy.cpp:
	echo > dummy.cpp

rcdir = $(kde_datadir)/kpovmodeler
rc_DATA =  kpovmodelershell.rc kpovmodelerui.rc kpovmodelerbrowser.rc \
	povraydocmap.xml baseinsertrules.xml questionmark.png


EXTRA_DIST = kpovmodeler.desktop


# make messages.po. Move this one to ../po/ and "make merge" in po
# the -x is for skipping messages already translated in kdelibs
messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kpovmodeler.pot

KDE_ICON = kpovmodeler_doc kpovmodeler

xdg_apps_DATA = kpovmodeler.desktop
