/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2003 by Andreas Zehender
    email                : zehender@kde.org
**************************************************************************

**************************************************************************
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation; either version 2 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
**************************************************************************/


// include files for QT
#include <qapplication.h>
#include <qdir.h>
#include <qstrlist.h>
#include <qclipboard.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qdatetime.h>
#include <qstringlist.h>

// include files for KDE
#include <kiconloader.h>
#include <kconfig.h>
#include <kstdaction.h>
#include <kaction.h>
#include <kmessagebox.h>
#include <ktempfile.h>
#include <kio/netaccess.h>
#include <kstandarddirs.h>
#include <kfilterdev.h>
#include <kfiledialog.h>

// application specific includes
#include "pmpart.h"
#include "pmshell.h"
#include "pmview.h"
#include "pmglview.h"
#include "pmallcommands.h"
#include "pmpovraywidget.h"
#include "pmpovrayrenderwidget.h"

#include "pmallobjects.h"
#include "pmcommandmanager.h"
#include "pmobjectdrag.h"
#include "pmxmlparser.h"
#include "pmpovrayparser.h"
#include "pmerrordialog.h"
#include "pmsettingsdialog.h"
#include "pminserterrordialog.h"
#include "pminsertpopup.h"

#include "pmpovray35format.h"
#include "pmserializer.h"

#include "pmfactory.h"
#include "pmdefaults.h"
#include "pmsymboltable.h"

#include "pmrendermodesdialog.h"
#include "pmrendermode.h"
#include "pmpovrayoutputwidget.h"
#include "pmrendermanager.h"
#include "pmdialogeditbase.h"
#include "pmdocumentationmap.h"
#include "pmlibrarymanager.h"
#include "pmlibraryhandleedit.h"
#include "pmlibraryobject.h"
#include "pmlibrarybrowser.h"
#include "pmlibraryobjectsearch.h"
#include "pmscene.h"

#include "pmpluginmanager.h"
#include "pminsertrulesystem.h"
#include "pmprototypemanager.h"
#include "pmiomanager.h"

#include "pmactions.h"
#include "pmrecursiveobjectiterator.h"

#include "pmerrorflags.h"

#include "pmfiledialog.h"

#ifdef PMEnableSimpleProfiling
QTime PMDebugTime;
#endif

//#define KPM_WITH_OBJECT_LIBRARY

PMPart::PMPart( QWidget* parentWidget, const char* widgetName,
                QObject* parent, const char* name, bool readwrite,
                PMShell* shell )
      : DCOPObject( "PMPartIface" ),
        KParts::ReadWritePart( parent, name ),
        m_commandManager( this )
{
   setPluginLoadingMode( LoadPlugins );
   setInstance( PMFactory::instance( ), false );
   m_pExtension = new PMBrowserExtension( this );

   m_pActiveObject = 0;
   m_canDecode = false;
   m_pScene = 0;
   m_pNewSelection = 0;
   m_sortedListUpToDate = false;
   m_numAddedObjects = 0;
   m_numInsertErrors = 0;
   m_pSymbolTable = 0;
   m_bCameraListUpToDate = true;
   m_updateNewObjectActions = false;
   m_pPovrayWidget = 0;
   m_pView = 0;
   m_pShell = shell;
   m_controlPoints.setAutoDelete( true );
   m_onlyCopyPaste = true;

   // call inits to invoke all other construction parts
   setReadWrite( readwrite );

   if( isReadWrite( ) )
      setXMLFile( "kpovmodelerui.rc" );
   else
      setXMLFile( "kpovmodelerbrowser.rc" );

   m_pPrototypeManager = new PMPrototypeManager( this );
   m_pInsertRuleSystem = new PMInsertRuleSystem( this );
   m_pIOManager = new PMIOManager( this );
   m_pInsertRuleSystem->loadRules( "baseinsertrules.xml" );

   initActions( );
   initDocument( );
   initView( parentWidget, widgetName );

   restoreConfig( instance( )->config( ) );

   connect( qApp->clipboard( ), SIGNAL( dataChanged( ) ),
            SLOT( slotClipboardDataChanged( ) ) );
   slotClipboardDataChanged( );
   connect( &m_commandManager, SIGNAL( updateUndoRedo( const QString&, const QString& ) ),
            SLOT( slotUpdateUndoRedo( const QString&, const QString& ) ) );
   connect( &m_commandManager, SIGNAL( objectChanged( PMObject*, const int, QObject* ) ),
            SLOT( slotObjectChanged( PMObject*, const int, QObject* ) ) );
   connect( &m_commandManager, SIGNAL( idChanged( PMObject*, const QString& ) ),
            SLOT( slotIDChanged( PMObject*, const QString& ) ) );

   PMPluginManager::theManager( )->registerPart( this );

   emit refresh( );
   slotObjectChanged( m_pScene, PMCNewSelection, this );
}

PMPart::PMPart( QWidget* /*parentWidget*/, const char* /*widgetName*/,
                QObject* parent, const char* name, bool readwrite,
                bool /*onlyCutPaste*/, PMShell* shell )
      : DCOPObject( "LibraryBrowserIface" ),
        KParts::ReadWritePart( parent, name ),
        m_commandManager( this )
{
   setPluginLoadingMode( LoadPlugins );
   setInstance( PMFactory::instance( ), false );

   m_pActiveObject = 0;
   m_canDecode = false;
   m_pNewSelection = 0;
   m_sortedListUpToDate = false;
   m_numAddedObjects = 0;
   m_numInsertErrors = 0;
   m_pSymbolTable = 0;
   m_bCameraListUpToDate = true;
   m_updateNewObjectActions = false;
   m_pPovrayWidget = 0;
   m_pView = 0;
   m_pShell = shell;
   m_pScene = new PMScene( this );
   m_onlyCopyPaste = true;

   // call inits to invoke all other construction parts
   setReadWrite( readwrite );

   if( isReadWrite( ) )
      setXMLFile( "kpovmodelerui.rc" );
   else
      setXMLFile( "kpovmodelerbrowser.rc" );

   m_pPrototypeManager = new PMPrototypeManager( this );
   m_pInsertRuleSystem = new PMInsertRuleSystem( this );
   m_pIOManager = new PMIOManager( this );
   m_pInsertRuleSystem->loadRules( "baseinsertrules.xml" );
   m_pSymbolTable = new PMSymbolTable( );

   initCopyPasteActions( );

   connect( &m_commandManager, SIGNAL( objectChanged( PMObject*, const int, QObject* ) ),
            SLOT( slotObjectChanged( PMObject*, const int, QObject* ) ) );

   PMPluginManager::theManager( )->registerPart( this );

   emit refresh( );
}

PMPart::~PMPart( )
{
   delete m_pExtension;
   deleteContents( );
   delete m_pSymbolTable;
   delete m_pPovrayWidget;
   PMPluginManager::theManager( )->removePart( this );
}

void PMPart::initCopyPasteActions( )
{
   // setup edit menu
   m_pCutAction = KStdAction::cut( this, SLOT( slotEditCut( ) ), actionCollection( ) );
   m_pCopyAction = KStdAction::copy( this, SLOT( slotEditCopy( ) ), actionCollection( ) );
   m_pPasteAction = KStdAction::paste( this, SLOT( slotEditPaste( ) ), actionCollection( ) );

   m_pDeleteAction =
      new KAction( i18n( "Delete" ), "edittrash", Qt::Key_Delete,
                   this, SLOT( slotEditDelete( ) ),
                   actionCollection( ), "edit_delete" );

   m_pCutAction->setEnabled( false );
   m_pCopyAction->setEnabled( false );
   m_pPasteAction->setEnabled( false );
   m_pDeleteAction->setEnabled( false );

}

void PMPart::initActions( )
{
   // file menu
   m_pImportAction = new KAction( i18n( "Import..." ), 0, this,
                                  SLOT( slotFileImport( ) ), actionCollection( ),
                                  "file_import" );
   m_pExportAction = new KAction( i18n( "&Export..." ), 0, this,
                                  SLOT( slotFileExport( ) ), actionCollection( ),
                                  "file_export" );

   initCopyPasteActions( );
   m_onlyCopyPaste = false;

   m_pRenderComboAction = new PMComboAction( i18n( "Render Modes" ), 0, this, SLOT( slotRenderMode( int ) ),
                                             actionCollection( ), "view_render_combo" );
   m_pRenderComboAction->setMinimumWidth( 250 );
   connect( m_pRenderComboAction, SIGNAL( plugged( ) ),
            SLOT( slotRenderModeActionPlugged( ) ) );
   m_pRenderAction = new KAction( i18n( "Render" ), "pmrender", 0, this, SLOT( slotRender( ) ),
                                  actionCollection( ), "view_render" );
   m_pRenderSettingsAction = new KAction( i18n( "Render Modes..." ), "pmrendersettings", 0, this, SLOT( slotRenderSettings( ) ),
                                          actionCollection( ), "view_render_settings" );
   m_pViewRenderWindowAction = new KAction( i18n( "Render Window" ), 0, this, SLOT( slotViewRenderWindow( ) ),
                                            actionCollection( ), "view_render_window" );
   m_pVisibilityLabelAction = new PMLabelAction( i18n( "Visibility level:" ) + QString( " " ), actionCollection( ), "view_visibility_label" );
   m_pVisibilityLevelAction = new PMSpinBoxAction( i18n( "Visibility Level" ),
                     0, this, SLOT( slotVisibilityLevelChanged( int ) ),
                     actionCollection( ), "view_visibility_level" );
   connect( m_pVisibilityLevelAction, SIGNAL( plugged( ) ),
            SLOT( slotVisibilityActionPlugged( ) ) );

   m_pGlobalDetailLabelAction = new PMLabelAction( i18n( "Global detail:" ) + QString( " " ), actionCollection( ), "global_detail_label" );
   m_pGlobalDetailAction = new KSelectAction( i18n("Global Detail Level"), KShortcut(), actionCollection(), "global_detail_level" );
   QStringList strList;
   strList.append( i18n( "Very Low" ) );
   strList.append( i18n( "Low" ) );
   strList.append( i18n( "Medium" ) );
   strList.append( i18n( "High" ) );
   strList.append( i18n( "Very High" ) );
   m_pGlobalDetailAction->setItems( strList );
   connect( m_pGlobalDetailAction, SIGNAL( activated( int ) ), SLOT( slotGlobalDetailLevelChanged( int ) ) );

   // new objects
   if( isReadWrite( ) )
   {
      m_pNewGlobalSettingsAction = new KAction( i18n( "Global Settings" ), "pmglobalsettings", 0, this, SLOT( slotNewGlobalSettings( ) ),
                                     actionCollection( ), "new_globalsettings" );
      m_readWriteActions.append( m_pNewGlobalSettingsAction );
      m_pNewSkySphereAction = new KAction( i18n( "Sky Sphere" ), "pmskysphere", 0, this, SLOT( slotNewSkySphere( ) ),
                                     actionCollection( ), "new_skysphere" );
      m_readWriteActions.append( m_pNewSkySphereAction );
      m_pNewRainbowAction = new KAction( i18n( "Rainbow" ), "pmrainbow", 0, this, SLOT( slotNewRainbow( ) ),
                                     actionCollection( ), "new_rainbow" );
      m_readWriteActions.append( m_pNewRainbowAction );
      m_pNewFogAction = new KAction( i18n( "Fog" ), "pmfog", 0, this, SLOT( slotNewFog( ) ),
                                     actionCollection( ), "new_fog" );
      m_readWriteActions.append( m_pNewFogAction );
      m_pNewInteriorAction = new KAction( i18n( "Interior" ), "pminterior", 0, this, SLOT( slotNewInterior( ) ),
                                     actionCollection( ), "new_interior" );
      m_readWriteActions.append( m_pNewInteriorAction );
      m_pNewMediaAction = new KAction( i18n( "Media" ), "pmmedia", 0, this, SLOT( slotNewMedia( ) ),
                                     actionCollection( ), "new_media" );
      m_readWriteActions.append( m_pNewMediaAction );
      m_pNewDensityAction = new KAction( i18n( "Density" ), "pmdensity", 0, this, SLOT( slotNewDensity( ) ),
                                     actionCollection( ), "new_density" );
      m_readWriteActions.append( m_pNewDensityAction );
      m_pNewMaterialAction = new KAction( i18n( "Material" ), "pmmaterial", 0, this, SLOT( slotNewMaterial( ) ),
                                     actionCollection( ), "new_material" );
      m_readWriteActions.append( m_pNewMaterialAction );
      m_pNewBoxAction = new KAction( i18n( "Box" ), "pmbox", 0, this, SLOT( slotNewBox( ) ),
                                     actionCollection( ), "new_box" );
      m_readWriteActions.append( m_pNewBoxAction );
      m_pNewSphereAction = new KAction( i18n( "Sphere" ), "pmsphere", 0, this, SLOT( slotNewSphere( ) ),
                                        actionCollection( ), "new_sphere" );
      m_readWriteActions.append( m_pNewSphereAction );
      m_pNewCylinderAction = new KAction( i18n( "Cylinder" ), "pmcylinder", 0, this, SLOT( slotNewCylinder( ) ),
                                          actionCollection( ), "new_cylinder" );
      m_readWriteActions.append( m_pNewCylinderAction );
      m_pNewConeAction =  new KAction( i18n( "Cone" ), "pmcone", 0, this, SLOT( slotNewCone( ) ),
                                       actionCollection( ), "new_cone" );
      m_readWriteActions.append( m_pNewConeAction );
      m_pNewTorusAction = new KAction( i18n( "Torus" ), "pmtorus", 0, this, SLOT( slotNewTorus( ) ),
                                       actionCollection( ), "new_torus" );
      m_readWriteActions.append( m_pNewTorusAction );

      m_pNewLatheAction = new KAction( i18n( "Lathe" ), "pmlathe", 0, this, SLOT( slotNewLathe( ) ),
                                       actionCollection( ), "new_lathe" );
      m_readWriteActions.append( m_pNewLatheAction );
      m_pNewPrismAction = new KAction( i18n( "Prism" ), "pmprism", 0, this, SLOT( slotNewPrism( ) ),
                                       actionCollection( ), "new_prism" );
      m_readWriteActions.append( m_pNewPrismAction );
      m_pNewSurfaceOfRevolutionAction = new KAction( i18n( "Surface of Revolution" ), "pmsor", 0, this, SLOT( slotNewSurfaceOfRevolution( ) ),
                                       actionCollection( ), "new_surfaceofrevolution" );
      m_readWriteActions.append( m_pNewSurfaceOfRevolutionAction );
      m_pNewSuperquadricEllipsoidAction = new KAction( i18n( "Superquadric Ellipsoid" ), "pmsqe", 0, this, SLOT( slotNewSuperquadricEllipsoid( ) ),
                                       actionCollection( ), "new_superquadricellipsoid" );
      m_readWriteActions.append( m_pNewSuperquadricEllipsoidAction );

      m_pNewJuliaFractalAction = new KAction( i18n( "Julia Fractal" ), "pmjuliafractal", 0, this, SLOT( slotNewJuliaFractal( ) ),
                                              actionCollection( ), "new_juliafractal" );
      m_readWriteActions.append( m_pNewJuliaFractalAction );
      m_pNewHeightFieldAction = new KAction( i18n( "Height Field" ), "pmheightfield", 0, this, SLOT( slotNewHeightField( ) ),
                                             actionCollection( ), "new_heightfield" );
      m_readWriteActions.append( m_pNewHeightFieldAction );
      m_pNewTextAction = new KAction( i18n( "Text" ), "pmtext", 0, this, SLOT( slotNewText( ) ),
                                      actionCollection( ), "new_text" );
      m_readWriteActions.append( m_pNewTextAction );

      m_pNewBlobAction = new KAction( i18n( "Blob" ), "pmblob", 0, this, SLOT( slotNewBlob( ) ),
                                      actionCollection( ), "new_blob" );
      m_readWriteActions.append( m_pNewBlobAction );
      m_pNewBlobSphereAction = new KAction( i18n( "Blob Sphere" ), "pmblobsphere", 0, this, SLOT( slotNewBlobSphere( ) ),
                                            actionCollection( ), "new_blobsphere" );
      m_readWriteActions.append( m_pNewBlobSphereAction );
      m_pNewBlobCylinderAction = new KAction( i18n( "Blob Cylinder" ), "pmblobcylinder", 0, this, SLOT( slotNewBlobCylinder( ) ),
                                              actionCollection( ), "new_blobcylinder" );
      m_readWriteActions.append( m_pNewBlobCylinderAction );

      m_pNewPlaneAction = new KAction( i18n( "Plane" ), "pmplane", 0, this, SLOT( slotNewPlane( ) ),
                                       actionCollection( ), "new_plane" );
      m_readWriteActions.append( m_pNewPlaneAction );
      m_pNewPolynomAction = new KAction( i18n( "Polynom" ), "pmpolynom", 0, this, SLOT( slotNewPolynom( ) ),
                                         actionCollection( ), "new_polynom" );
      m_readWriteActions.append( m_pNewPolynomAction );

      m_pNewDeclareAction = new KAction( i18n( "Declaration" ), "pmdeclare", 0, this, SLOT( slotNewDeclare( ) ),
                                         actionCollection( ), "new_declare" );
      m_readWriteActions.append( m_pNewDeclareAction );
      m_pNewObjectLinkAction = new KAction( i18n( "Object Link" ), "pmobjectlink", 0, this, SLOT( slotNewObjectLink( ) ),
                                            actionCollection( ), "new_objectlink" );
      m_readWriteActions.append( m_pNewObjectLinkAction );

      m_pNewUnionAction = new KAction( i18n( "Union" ), "pmunion", 0, this, SLOT( slotNewUnion( ) ),
                                       actionCollection( ), "new_union" );
      m_readWriteActions.append( m_pNewUnionAction );
      m_pNewIntersectionAction = new KAction( i18n( "Intersection" ), "pmintersection", 0, this, SLOT( slotNewIntersection( ) ),
                                              actionCollection( ), "new_intersection" );
      m_readWriteActions.append( m_pNewIntersectionAction );
      m_pNewDifferenceAction = new KAction( i18n( "Difference" ), "pmdifference", 0, this, SLOT( slotNewDifference( ) ),
                                            actionCollection( ), "new_difference" );
      m_readWriteActions.append( m_pNewDifferenceAction );
      m_pNewMergeAction = new KAction( i18n( "Merge" ), "pmmerge", 0, this, SLOT( slotNewMerge( ) ),
                                       actionCollection( ), "new_merge" );
      m_readWriteActions.append( m_pNewMergeAction );

      m_pNewBoundedByAction = new KAction( i18n( "Bounded By" ), "pmboundedby", 0, this, SLOT( slotNewBoundedBy( ) ),
                                       actionCollection( ), "new_boundedby" );
      m_readWriteActions.append( m_pNewBoundedByAction );
      m_pNewClippedByAction = new KAction( i18n( "Clipped By" ), "pmclippedby", 0, this, SLOT( slotNewClippedBy( ) ),
                                       actionCollection( ), "new_clippedby" );
      m_readWriteActions.append( m_pNewClippedByAction );

      m_pNewLightAction = new KAction( i18n( "Light" ), "pmlight", 0, this, SLOT( slotNewLight( ) ),
                                       actionCollection( ), "new_light" );
      m_readWriteActions.append( m_pNewLightAction );
      m_pNewLooksLikeAction = new KAction( i18n( "Looks Like" ), "pmlookslike", 0, this, SLOT( slotNewLooksLike( ) ),
                                       actionCollection( ), "new_lookslike" );
      m_readWriteActions.append( m_pNewLooksLikeAction );
      m_pNewProjectedThroughAction = new KAction( i18n( "Projected Through" ), "pmprojectedthrough", 0, this, SLOT( slotNewProjectedThrough( ) ),
                                       actionCollection( ), "new_projectedthrough" );
      m_readWriteActions.append( m_pNewProjectedThroughAction );

      m_pNewBicubicPatchAction = new KAction( i18n( "Bicubic Patch" ), "pmbicubicpatch", 0, this, SLOT( slotNewBicubicPatch( ) ),
                                              actionCollection( ), "new_bicubicpatch" );
      m_readWriteActions.append( m_pNewBicubicPatchAction );
      m_pNewDiscAction = new KAction( i18n( "Disc" ), "pmdisc", 0, this, SLOT( slotNewDisc( ) ),
                                      actionCollection( ), "new_disc" );
      m_readWriteActions.append( m_pNewDiscAction );
      m_pNewTriangleAction = new KAction( i18n( "Triangle" ), "pmtriangle", 0, this, SLOT( slotNewTriangle( ) ),
                                          actionCollection( ), "new_triangle" );
      m_readWriteActions.append( m_pNewTriangleAction );


      m_pNewCameraAction = new KAction( i18n( "Camera" ), "pmcamera", 0, this, SLOT( slotNewCamera( ) ),
                                        actionCollection( ), "new_camera" );
      m_readWriteActions.append( m_pNewCameraAction );

      m_pNewTextureAction = new KAction( i18n( "Texture" ), "pmtexture", 0, this, SLOT( slotNewTexture( ) ),
                                         actionCollection( ), "new_texture" );
      m_readWriteActions.append( m_pNewTextureAction );

      m_pNewPigmentAction = new KAction( i18n( "Pigment" ), "pmpigment", 0, this, SLOT( slotNewPigment( ) ),
                                         actionCollection( ), "new_pigment" );
      m_readWriteActions.append( m_pNewPigmentAction );
      m_pNewNormalAction = new KAction( i18n( "Normal" ), "pmnormal", 0, this, SLOT( slotNewNormal( ) ),
                                         actionCollection( ), "new_normal" );
      m_readWriteActions.append( m_pNewNormalAction );
      m_pNewSolidColorAction = new KAction( i18n( "Solid Color" ), "pmsolidcolor", 0, this, SLOT( slotNewSolidColor( ) ),
                                            actionCollection( ), "new_solidcolor" );
      m_readWriteActions.append( m_pNewSolidColorAction );

      m_pNewTextureListAction = new KAction( i18n( "Texture List" ), "pmtexturelist", 0, this, SLOT( slotNewTextureList( ) ),
                                            actionCollection( ), "new_texturelist" );
      m_readWriteActions.append( m_pNewTextureListAction );
      m_pNewColorListAction = new KAction( i18n( "Color List" ), "pmcolorlist", 0, this, SLOT( slotNewColorList( ) ),
                                            actionCollection( ), "new_colorlist" );
      m_readWriteActions.append( m_pNewColorListAction );
      m_pNewPigmentListAction = new KAction( i18n( "Pigment List" ), "pmpigmentlist", 0, this, SLOT( slotNewPigmentList( ) ),
                                            actionCollection( ), "new_pigmentlist" );
      m_readWriteActions.append( m_pNewPigmentListAction );
      m_pNewNormalListAction = new KAction( i18n( "Normal List" ), "pmnormallist", 0, this, SLOT( slotNewNormalList( ) ),
                                            actionCollection( ), "new_normallist" );
      m_readWriteActions.append( m_pNewNormalListAction );
      m_pNewDensityListAction = new KAction( i18n( "Density List" ), "pmdensitylist", 0, this, SLOT( slotNewDensityList( ) ),
                                            actionCollection( ), "new_densitylist" );
      m_readWriteActions.append( m_pNewDensityListAction );

      m_pNewFinishAction = new KAction( i18n( "Finish" ), "pmfinish", 0, this, SLOT( slotNewFinish( ) ),
                                            actionCollection( ), "new_finish" );
      m_readWriteActions.append( m_pNewFinishAction );

      m_pNewPatternAction = new KAction( i18n( "Pattern" ), "pmpattern", 0, this, SLOT( slotNewPattern( ) ),
                                         actionCollection( ), "new_pattern" );
      m_readWriteActions.append( m_pNewPatternAction );
      m_pNewBlendMapModifiersAction = new KAction( i18n( "Blend Map Modifiers" ), "pmblendmapmodifiers", 0, this, SLOT( slotNewBlendMapModifiers( ) ),
                                         actionCollection( ), "new_blendmapmodifiers" );
      m_readWriteActions.append( m_pNewBlendMapModifiersAction );
      m_pNewTextureMapAction = new KAction( i18n( "Texture Map" ), "pmtexturemap", 0, this, SLOT( slotNewTextureMap( ) ),
                                            actionCollection( ), "new_texturemap" );
      m_readWriteActions.append( m_pNewTextureMapAction );
      m_pNewMaterialMapAction = new KAction( i18n( "Material Map" ), "pmmaterialmap", 0, this, SLOT( slotNewMaterialMap( ) ),
                                            actionCollection( ), "new_materialmap" );
      m_readWriteActions.append( m_pNewMaterialMapAction );
      m_pNewPigmentMapAction = new KAction( i18n( "Pigment Map" ), "pmpigmentmap", 0, this, SLOT( slotNewPigmentMap( ) ),
                                            actionCollection( ), "new_pigmentmap" );
      m_readWriteActions.append( m_pNewPigmentMapAction );
      m_pNewColorMapAction = new KAction( i18n( "Color Map" ), "pmcolormap", 0, this, SLOT( slotNewColorMap( ) ),
                                          actionCollection( ), "new_colormap" );
      m_readWriteActions.append( m_pNewColorMapAction );
      m_pNewNormalMapAction = new KAction( i18n( "Normal Map" ), "pmnormalmap", 0, this, SLOT( slotNewNormalMap( ) ),
                                           actionCollection( ), "new_normalmap" );
      m_readWriteActions.append( m_pNewNormalMapAction );
      m_pNewBumpMapAction = new KAction( i18n( "Bump Map" ), "pmbumpmap", 0, this, SLOT( slotNewBumpMap( ) ),
                                           actionCollection( ), "new_bumpmap" );
      m_readWriteActions.append( m_pNewBumpMapAction );
      m_pNewSlopeMapAction = new KAction( i18n( "Slope Map" ), "pmslopemap", 0, this, SLOT( slotNewSlopeMap( ) ),
                                           actionCollection( ), "new_slopemap" );
      m_readWriteActions.append( m_pNewSlopeMapAction );
      m_pNewDensityMapAction = new KAction( i18n( "Density Map" ), "pmdensitymap", 0, this, SLOT( slotNewDensityMap( ) ),
                                           actionCollection( ), "new_densitymap" );
      m_readWriteActions.append( m_pNewDensityMapAction );
      m_pNewSlopeAction = new KAction( i18n( "Slope" ), "pmslope", 0, this, SLOT( slotNewSlope( ) ),
                                           actionCollection( ), "new_slope" );
      m_readWriteActions.append( m_pNewSlopeAction );

      m_pNewWarpAction = new KAction( i18n( "Warp" ), "pmwarp", 0, this, SLOT( slotNewWarp( ) ),
                                            actionCollection( ), "new_warp" );
      m_readWriteActions.append( m_pNewWarpAction );
      m_pNewImageMapAction = new KAction( i18n( "Image Map" ), "pmimagemap", 0, this, SLOT( slotNewImageMap( ) ),
                                            actionCollection( ), "new_imagemap" );
      m_readWriteActions.append( m_pNewImageMapAction );
      m_pNewQuickColorAction = new KAction( i18n( "QuickColor" ), "pmquickcolor", 0, this, SLOT( slotNewQuickColor( ) ),
                                             actionCollection( ), "new_quickcolor" );
      m_readWriteActions.append( m_pNewQuickColorAction );

      m_pNewTranslateAction = new KAction( i18n( "Translate" ), "pmtranslate", 0, this, SLOT( slotNewTranslate( ) ),
                                     actionCollection( ), "new_translate" );
      m_readWriteActions.append( m_pNewTranslateAction );
      m_pNewScaleAction = new KAction( i18n( "Scale" ), "pmscale", 0, this, SLOT( slotNewScale( ) ),
                                     actionCollection( ), "new_scale" );
      m_readWriteActions.append( m_pNewScaleAction );
      m_pNewRotateAction = new KAction( i18n( "Rotate" ), "pmrotate", 0, this, SLOT( slotNewRotate( ) ),
                                     actionCollection( ), "new_rotate" );
      m_readWriteActions.append( m_pNewRotateAction );
      m_pNewMatrixAction = new KAction( i18n( "Matrix" ), "pmmatrix", 0, this, SLOT( slotNewMatrix( ) ),
                                        actionCollection( ), "new_povraymatrix" );
      m_readWriteActions.append( m_pNewMatrixAction );

      m_pNewCommentAction = new KAction( i18n( "Comment" ), "pmcomment", 0, this, SLOT( slotNewComment( ) ),
                                         actionCollection( ), "new_comment" );
      m_readWriteActions.append( m_pNewCommentAction );
      m_pNewRawAction = new KAction( i18n( "Raw Povray" ), "pmraw", 0, this, SLOT( slotNewRaw( ) ),
                                         actionCollection( ), "new_raw" );
      m_readWriteActions.append( m_pNewRawAction );

      // POV-Ray 3.5 objects
      m_pNewIsoSurfaceAction = new KAction( i18n( "Iso Surface" ), "pmisosurface", 0, this, SLOT( slotNewIsoSurface( ) ),
                                         actionCollection( ), "new_isosurface" );
      m_readWriteActions.append( m_pNewIsoSurfaceAction );
      m_pNewRadiosityAction = new KAction( i18n( "Radiosity" ), "pmradiosity", 0, this, SLOT( slotNewRadiosity( ) ),
                                         actionCollection( ), "new_radiosity" );
      m_readWriteActions.append( m_pNewRadiosityAction );
      m_pNewGlobalPhotonsAction = new KAction( i18n( "Global Photons" ), "pmglobalphotons", 0, this, SLOT( slotNewGlobalPhotons( ) ),
                                         actionCollection( ), "new_globalphotons" );
      m_readWriteActions.append( m_pNewGlobalPhotonsAction );
      m_pNewPhotonsAction = new KAction( i18n( "Photons" ), "pmphotons", 0, this, SLOT( slotNewPhotons( ) ),
                                         actionCollection( ), "new_photons" );
      m_readWriteActions.append( m_pNewPhotonsAction );
      m_pNewLightGroupAction = new KAction( i18n( "Light Group" ), "pmlightgroup", 0, this, SLOT( slotNewLightGroup( ) ),
                                         actionCollection( ), "new_lightgroup" );
      m_readWriteActions.append( m_pNewLightGroupAction );
      m_pNewInteriorTextureAction = new KAction( i18n( "Interior Texture" ), "pminteriortexture", 0, this, SLOT( slotNewInteriorTexture( ) ),
                                         actionCollection( ), "new_interiortexture" );
      m_readWriteActions.append( m_pNewInteriorTextureAction );
      m_pNewSphereSweepAction = new KAction( i18n( "Sphere Sweep" ), "pmspheresweep", 0, this, SLOT( slotNewSphereSweep( ) ),
                                         actionCollection( ), "new_spheresweep" );
      m_readWriteActions.append( m_pNewSphereSweepAction );
      m_pNewMeshAction = new KAction( i18n( "Mesh" ), "pmmesh", 0, this, SLOT( slotNewMesh( ) ),
                                         actionCollection( ), "new_mesh" );
      m_readWriteActions.append( m_pNewMeshAction );

#ifdef KPM_WITH_OBJECT_LIBRARY
      m_pSearchLibraryObjectAction = new KAction( i18n( "Search Object" ), "pmsearchlibrary", 0, this, SLOT( slotSearchLibraryObject( ) ),
                                         actionCollection( ), "search_library_object" );
      m_readWriteActions.append( m_pSearchLibraryObjectAction );
#endif

      m_pUndoAction = KStdAction::undo( this, SLOT( slotEditUndo( ) ), actionCollection( ) );
      m_pRedoAction = KStdAction::redo( this, SLOT( slotEditRedo( ) ), actionCollection( ) );
      m_pUndoAction->setEnabled( false );
      m_pRedoAction->setEnabled( false );
   }
   else
   {
      m_pNewGlobalSettingsAction = 0;
      m_pNewSkySphereAction = 0;
      m_pNewRainbowAction = 0;
      m_pNewFogAction = 0;

      m_pNewInteriorAction = 0;
      m_pNewMediaAction = 0;
      m_pNewDensityAction = 0;
      m_pNewMaterialAction = 0;
      m_pNewBoxAction = 0;
      m_pNewSphereAction = 0;
      m_pNewCylinderAction = 0;
      m_pNewConeAction = 0;
      m_pNewTorusAction = 0;
      m_pNewLatheAction = 0;
      m_pNewPrismAction = 0;
      m_pNewSurfaceOfRevolutionAction = 0;
      m_pNewSuperquadricEllipsoidAction = 0;
      m_pNewJuliaFractalAction = 0;
      m_pNewHeightFieldAction = 0;
      m_pNewTextAction = 0;

      m_pNewBlobAction = 0;
      m_pNewBlobSphereAction = 0;
      m_pNewBlobCylinderAction = 0;

      m_pNewPlaneAction = 0;
      m_pNewPolynomAction = 0;

      m_pNewDeclareAction = 0;
      m_pNewObjectLinkAction = 0;

      m_pNewDiscAction = 0;
      m_pNewBicubicPatchAction = 0;
      m_pNewTriangleAction = 0;

      m_pNewUnionAction = 0;
      m_pNewDifferenceAction = 0;
      m_pNewIntersectionAction = 0;
      m_pNewMergeAction = 0;

      m_pNewBoundedByAction = 0;
      m_pNewClippedByAction = 0;

      m_pNewLightAction = 0;
      m_pNewLooksLikeAction = 0;
      m_pNewProjectedThroughAction = 0;

      m_pNewCameraAction = 0;

      m_pNewTextureAction = 0;
      m_pNewPigmentAction = 0;
      m_pNewNormalAction = 0;
      m_pNewSolidColorAction = 0;
      m_pNewFinishAction = 0;
      m_pNewTextureListAction = 0;
      m_pNewColorListAction = 0;
      m_pNewPigmentListAction = 0;
      m_pNewNormalListAction = 0;
      m_pNewDensityListAction = 0;

      m_pNewPatternAction = 0;
      m_pNewBlendMapModifiersAction = 0;
      m_pNewTextureMapAction = 0;
      m_pNewMaterialMapAction = 0;
      m_pNewPigmentMapAction = 0;
      m_pNewColorMapAction = 0;
      m_pNewNormalMapAction = 0;
      m_pNewBumpMapAction = 0;
      m_pNewSlopeMapAction = 0;
      m_pNewDensityMapAction = 0;

      m_pNewWarpAction = 0;
      m_pNewImageMapAction = 0;
      m_pNewSlopeAction = 0;

      m_pNewTranslateAction = 0;
      m_pNewScaleAction = 0;
      m_pNewRotateAction = 0;
      m_pNewMatrixAction = 0;
      m_pNewCommentAction = 0;
      m_pNewRawAction = 0;

      m_pNewIsoSurfaceAction = 0;
      m_pNewRadiosityAction = 0;
      m_pNewGlobalPhotonsAction = 0;
      m_pNewPhotonsAction = 0;
      m_pNewLightGroupAction = 0;
      m_pNewInteriorTextureAction = 0;
      m_pNewSphereSweepAction = 0;
      m_pNewMeshAction = 0;

      // POV-Ray

      m_pUndoAction = 0;
      m_pRedoAction = 0;
   }
}

void PMPart::updateNewObjectActions( )
{
   if( isReadWrite( ) && !m_onlyCopyPaste )
   {
      QPtrListIterator<PMMetaObject> it =
         m_pPrototypeManager->prototypeIterator( );
      KAction* action;
      bool enable;
      bool readWriteParent = false;

      if( m_pActiveObject )
         if( m_pActiveObject->parent( ) )
            if( !m_pActiveObject->parent( )->isReadOnly( ) )
               readWriteParent = true;

      for( ; it.current( ); ++it )
      {
         // get the action object for that type of PMObject
         // action names are "new_" + povray name
         // (like new_box, new_sphere ...)

         QString actionName = "new_" + it.current( )->className( ).lower( );
         action = actionCollection( )->action( actionName.latin1( ) );
         if( action )
         {
            if( m_pActiveObject )
            {
               QString insertName = it.current( )->className( );
               enable = m_pActiveObject->canInsert( insertName, 0 );
               if( !enable )
                  if( m_pActiveObject->lastChild( ) )
                     enable = m_pActiveObject->canInsert( insertName, m_pActiveObject->lastChild( ) );
               if( !enable )
                  if( readWriteParent )
                     enable |= m_pActiveObject->parent( )->canInsert( insertName, m_pActiveObject );
            }
            else
               enable = false;
            action->setEnabled( enable );
         }
      }
      // special treatment for csg actions
      if( m_pActiveObject )
      {
         enable = m_pActiveObject->canInsert( QString( "CSG" ), 0 );
         if( !enable )
            if( m_pActiveObject->lastChild( ) )
               enable = m_pActiveObject->canInsert( QString( "CSG" ), m_pActiveObject->lastChild( ) );
         if( !enable )
            if( readWriteParent )
               enable = m_pActiveObject->parent( )->canInsert( QString( "CSG" ), m_pActiveObject );
      }
      else
         enable = false;
      m_pNewUnionAction->setEnabled( enable );
      m_pNewIntersectionAction->setEnabled( enable );
      m_pNewDifferenceAction->setEnabled( enable );
      m_pNewMergeAction->setEnabled( enable );
   }
   m_updateNewObjectActions = false;
}

void PMPart::disableReadWriteActions( )
{
   QPtrListIterator<KAction> it( m_readWriteActions);
   for( ; it.current( ); ++it )
      it.current( )->setEnabled( false );
}

void PMPart::initDocument( )
{
   newDocument( );
}

void PMPart::initView( QWidget* parent, const char* name )
{
   if( !m_pShell )
   {
      // a part inside konqueror
      // simple layout
      m_pView = new PMView( this, parent, name );
      m_pView->show( );
      setWidget( m_pView );
   }
   else
   {
      // inside a PMShell
      // the shell will create the view
   }
}

void PMPart::saveConfig( KConfig* cfg )
{
   if( m_pView )
      m_pView->saveConfig( cfg );
   PMErrorDialog::saveConfig( cfg );
   PMRenderModesDialog::saveConfig( cfg );
   PMRenderModeDialog::saveConfig( cfg );
   PMPovrayOutputWidget::saveConfig( cfg );
   PMRenderManager::theManager( )->saveConfig( cfg );
   PMGLView::saveConfig( cfg );
   PMDialogEditBase::saveConfig( cfg );
   PMControlPoint::saveConfig( cfg );
   PMPovrayRenderWidget::saveConfig( cfg );
   PMSettingsDialog::saveConfig( cfg );
   PMLibraryHandleEdit::saveConfig( cfg );
   PMDocumentationMap::theMap( )->saveConfig( cfg );
   PMLibraryManager::theManager( )->saveConfig(cfg );

   cfg->setGroup( "Rendering" );
   cfg->writeEntry( "SphereUSteps", PMSphere::uSteps( ) );
   cfg->writeEntry( "SphereVSteps", PMSphere::vSteps( ) );
   cfg->writeEntry( "CylinderSteps", PMCylinder::steps( ) );
   cfg->writeEntry( "ConeSteps", PMCone::steps( ) );
   cfg->writeEntry( "DiscSteps", PMDisc::steps( ) );
   cfg->writeEntry( "BlobSphereUSteps", PMBlobSphere::uSteps( ) );
   cfg->writeEntry( "BlobSphereVSteps", PMBlobSphere::vSteps( ) );
   cfg->writeEntry( "BlobCylinderUSteps", PMBlobCylinder::uSteps( ) );
   cfg->writeEntry( "BlobCylinderVSteps", PMBlobCylinder::vSteps( ) );
   cfg->writeEntry( "TorusUSteps", PMTorus::uSteps( ) );
   cfg->writeEntry( "TorusVSteps", PMTorus::vSteps( ) );
   cfg->writeEntry( "LatheSSteps", PMLathe::sSteps( ) );
   cfg->writeEntry( "LatheRSteps", PMLathe::rSteps( ) );
   cfg->writeEntry( "SorSSteps", PMSurfaceOfRevolution::sSteps( ) );
   cfg->writeEntry( "SorRSteps", PMSurfaceOfRevolution::rSteps( ) );
   cfg->writeEntry( "PrismSSteps", PMPrism::sSteps( ) );
   cfg->writeEntry( "PlaneSize", PMPlane::planeSize( ) );
   cfg->writeEntry( "SqeUSteps", PMSuperquadricEllipsoid::uSteps( ) );
   cfg->writeEntry( "SqeVSteps", PMSuperquadricEllipsoid::vSteps( ) );
   cfg->writeEntry( "SphereSweepRSteps", PMSphereSweep::rSteps( ) );
   cfg->writeEntry( "SphereSweepSSteps", PMSphereSweep::sSteps( ) );
   cfg->writeEntry( "HeightFieldVariance", PMHeightField::variance( ) );
   cfg->writeEntry( "GlobalDetailLevel", PMDetailObject::globalDetailLevel( ) );

   cfg->writeEntry( "DirectRendering", PMGLView::isDirectRenderingEnabled( ) );
}

void PMPart::restoreConfig( KConfig* cfg )
{
   if( m_pView )
      m_pView->restoreConfig( cfg );
   PMErrorDialog::restoreConfig( cfg );
   PMRenderModesDialog::restoreConfig( cfg );
   PMRenderModeDialog::restoreConfig( cfg );
   PMPovrayOutputWidget::restoreConfig( cfg );
   PMRenderManager::theManager( )->restoreConfig( cfg );
   PMGLView::restoreConfig( cfg );
   PMDialogEditBase::restoreConfig( cfg );
   PMControlPoint::restoreConfig( cfg );
   PMPovrayRenderWidget::restoreConfig( cfg );
   PMSettingsDialog::restoreConfig( cfg );
   PMLibraryHandleEdit::restoreConfig( cfg );
   PMDocumentationMap::theMap( )->restoreConfig( cfg );
   PMLibraryManager::theManager( )->restoreConfig(cfg );

   cfg->setGroup( "Rendering" );
   PMSphere::setUSteps( cfg->readNumEntry( "SphereUSteps", c_defaultSphereUSteps ) );
   PMSphere::setVSteps( cfg->readNumEntry( "SphereVSteps", c_defaultSphereVSteps ) );
   PMCylinder::setSteps( cfg->readNumEntry( "CylinderSteps", c_defaultCylinderSteps ) );
   PMCone::setSteps( cfg->readNumEntry( "ConeSteps", c_defaultConeSteps ) );
   PMTorus::setUSteps( cfg->readNumEntry( "TorusUSteps", c_defaultTorusUSteps ) );
   PMTorus::setVSteps( cfg->readNumEntry( "TorusVSteps", c_defaultTorusVSteps ) );
   PMLathe::setSSteps( cfg->readNumEntry( "LatheSSteps", c_defaultLatheSSteps ) );
   PMLathe::setRSteps( cfg->readNumEntry( "LatheRSteps", c_defaultLatheRSteps ) );
   PMSurfaceOfRevolution::setSSteps( cfg->readNumEntry( "SorSSteps", c_defaultSurfaceOfRevolutionSSteps ) );
   PMSurfaceOfRevolution::setRSteps( cfg->readNumEntry( "SorRSteps", c_defaultSurfaceOfRevolutionRSteps ) );
   PMPrism::setSSteps( cfg->readNumEntry( "PrismSSteps", c_defaultPrismSSteps ) );
   PMPlane::setPlaneSize( cfg->readDoubleNumEntry( "PlaneSize", c_defaultPlaneSize ) );
   PMDisc::setSteps( cfg->readNumEntry( "DiscSteps", c_defaultDiscSteps ) );
   PMBlobSphere::setUSteps( cfg->readNumEntry( "BlobSphereUSteps", c_defaultBlobSphereUSteps ) );
   PMBlobSphere::setVSteps( cfg->readNumEntry( "BlobSphereVSteps", c_defaultBlobSphereVSteps ) );
   PMBlobCylinder::setUSteps( cfg->readNumEntry( "BlobCylinderUSteps", c_defaultBlobCylinderUSteps ) );
   PMBlobCylinder::setVSteps( cfg->readNumEntry( "BlobCylinderVSteps", c_defaultBlobCylinderVSteps ) );
   PMSuperquadricEllipsoid::setUSteps( cfg->readNumEntry( "SqeUSteps", c_defaultSuperquadricEllipsoidUSteps ) );
   PMSuperquadricEllipsoid::setVSteps( cfg->readNumEntry( "SqeVSteps", c_defaultSuperquadricEllipsoidVSteps ) );
   PMSphereSweep::setRSteps( cfg->readNumEntry( "SphereSweepRSteps", c_defaultSphereSweepRSteps ) );
   PMSphereSweep::setSSteps( cfg->readNumEntry( "SphereSweepSSteps", c_defaultSphereSweepSSteps ) );
   PMHeightField::setVariance( cfg->readNumEntry( "HeightFieldVariance", c_defaultHeightFieldVariance ) );
   PMDetailObject::setGlobalDetailLevel( cfg->readNumEntry( "GlobalDetailLevel", c_defaultDetailObjectGlobalDetailLevel ) );
   m_pGlobalDetailAction->setCurrentItem( PMDetailObject::globalDetailLevel( ) - 1 );

   if( PMGLView::isDirectRenderingEnabled( ) ) // otherwise it was disabled with a command line switch
      PMGLView::enableDirectRendering( cfg->readBoolEntry( "DirectRendering", true ) );
}

bool PMPart::openFile( )
{
   QIODevice* dev = KFilterDev::deviceForFile( m_file, "application/x-gzip" );
   bool success = true;
   PMObjectList list;

   deleteContents( );

   setModified( false );

   if( dev && dev->open( IO_ReadOnly ) )
   {
      PMXMLParser parser( this, dev );

      parser.parse( &list, 0, 0 );

      if( parser.errors( ) || parser.warnings( ) )
      {
         PMErrorDialog dlg( parser.messages( ), parser.errorFlags( ) );
         // still try to insert the correct parsed objects?
         success = ( dlg.exec( ) == QDialog::Accepted );
      }
      if( success )
      {
         PMObject* obj = list.first( );
         if( obj )
         {
            if( obj->type( ) == "Scene" )
               m_pScene = ( PMScene* ) obj;
            else
               success = false;
         }
         else
            success = false;
      }
   }
   else
      success = false;

   if( !success )
   {
      m_url = KURL( );
      newDocument( );
   }

   m_pScene->setReadOnly( !isReadWrite( ) );
   if( !isReadWrite( ) )
      disableReadWriteActions( );
   m_bCameraListUpToDate = false;

   emit refresh( );
   updateRenderModes( );
   updateVisibilityLevel( );
   slotObjectChanged( m_pScene, PMCNewSelection, this );

   if( dev )
      delete dev;

   return success;
}

bool PMPart::saveFile( )
{
   bool success = false;

   QIODevice* dev = KFilterDev::deviceForFile( m_file, "application/x-gzip" );
   if( dev && dev->open( IO_WriteOnly ) )
   {
      QDomDocument doc( "KPOVMODELER" );
      QDomElement e = ( ( PMObject* )m_pScene)->serialize( doc );
      doc.appendChild( e );

      QTextStream str( dev );
      str << doc;
      dev->close( );
      setModified( false );
      success = true;
   }

   if( dev )
      delete dev;

   return success;
}

bool PMPart::exportPovray( const KURL& url )
{
   KTempFile* tempFile = 0;
   QFile* file = 0;
   bool ok = true;

   if( !url.isValid( ) )
      return false;

   if( url.isLocalFile( ) )
   {
      // Local file
      file = new QFile( url.path( ) );
      if( !file->open( IO_WriteOnly ) )
         ok = false;
   }
   else
   {
      // Remote file
      // provide a temp file
      tempFile = new KTempFile( );
      if( tempFile->status( ) != 0 )
         ok = false;
      else
         file = tempFile->file( );
   }

   if( ok )
   {
      PMPovray35Format format;
      PMSerializer* dev = format.newSerializer( file );
      dev->serialize( m_pScene );
      delete dev;

      if( tempFile )
      {
         tempFile->close( );
         ok = KIO::NetAccess::upload( tempFile->name( ), url, (QWidget*) 0 );
         tempFile->unlink( );
         file = 0;
      }
      else
         file->close( );
   }

   delete file;
   delete tempFile;

   return ok;
}

QString PMPart::activeObjectName( )
{
   QString result = "";
   PMObject* tmpObj;
   PMObject* testSib;
   int idx = 0;

   tmpObj = activeObject( );
   while( tmpObj != m_pScene && tmpObj )
   {
      // count previous siblings of the same type (for array like entries)
      testSib = tmpObj;
      while( ( testSib = testSib->prevSibling( ) ) )
         if( testSib->type( ) == tmpObj->type( ) )
            idx++;

      // prepend to result
      if( idx != 0 )
         result = tmpObj->type( ) + "[" + QString::number( idx ) + "]/" + result;
      else
         result = tmpObj->type( ) + "/" + result;

      // go up in the scene
      tmpObj = tmpObj->parent( );
      idx = 0;
   }

   // Make the object name an absolute name
   result = "/" + result;

   return result;
}

bool PMPart::setActiveObject( const QString& name )
{
   PMObject* tmpObj;
   PMObject* tmpSibling;
   int pos, siblingIndex, objIndex, firstBracket, lastBracket;
   QString objPath;
   QString pathElem;

   // check if it's a absolute object name
   if( name[0] == '/' )
   {
      tmpObj = m_pScene;
      objPath = name.mid( 1 );  // clear that first character
   }
   else
      tmpObj = activeObject( );

   // get the first element
   pos = objPath.find( '/' );
   if( pos != -1 )
   {
      pathElem = objPath.mid( 0, pos );
      objPath = objPath.mid( pos + 1 );
   }
   else
   {
      pathElem = objPath;
      objPath = QString::null;
   }

   while( !pathElem.isNull( ) )
   {
      if( !pathElem.isEmpty( ) )
      {
         // Special treatment for brackets
         firstBracket = pathElem.find( '[' );
         if( firstBracket != -1 )
         {
            lastBracket = pathElem.findRev( ']' );
            objIndex = pathElem.mid( firstBracket + 1, lastBracket - firstBracket - 1).toInt( );
            pathElem = pathElem.left( firstBracket );
         }
         else
            objIndex = 0;

         // Iterate the children for this element. We stop when there are no more siblings
         // or the object is of the correct type and it's index count is also correct
         siblingIndex = 0;
         tmpSibling = tmpObj->firstChild( );
         for( ; tmpSibling && ( tmpSibling->type( ) != pathElem || siblingIndex != objIndex );
                                                                tmpSibling = tmpSibling->nextSibling( ) )
         {
            // Found an object of the type we are looking for
            if( tmpSibling->type( ) == pathElem )
               siblingIndex++;
         }
         if( tmpSibling )
            tmpObj = tmpSibling;
         else
            // No correct sibling
            return false;

      }

      // Get the next element
      pos = objPath.find( '/' );
      if( pos != -1 )
      {
         pathElem = objPath.mid( 0, pos );
         objPath = objPath.mid( pos + 1 );
      }
      else
      {
         pathElem = objPath;
         objPath = QString::null;
      }
   }
   if( tmpObj )
   {
      slotObjectChanged( tmpObj, PMCNewSelection, this );
      return true;
   }
   else
      return false;
}

QStringList PMPart::getProperties( )
{
   PMObject* curObj = activeObject( );

   // Ensure that there is an active object
   if( !curObj )
      return QStringList( );

   return curObj->properties( );
}

bool PMPart::setProperty( const QString& name, const PMVariant& value )
{
   PMObject* curObj = activeObject( );

   // Ensure that there is an active object
   if( !curObj )
      return false;

   curObj->setProperty( name, value );
   slotObjectChanged( curObj, PMCNewSelection, this );
   return true;
}

bool PMPart::setProperty( const QString& name, const QString& value )
{
   PMObject* curObj = activeObject( );
   PMVariant variant;

   // Ensure that there is an active object
   if( !curObj )
      return false;

   variant.fromString( curObj->property( name ).dataType( ), value );
   curObj->setProperty( name, variant );
   slotObjectChanged( curObj, PMCNewSelection, this );
   return true;
}

const PMVariant PMPart::getProperty( const QString& name )
{
   PMObject* curObj = activeObject( );

   // Ensure that there is an active object
   if( !curObj )
      return PMVariant( );

   return curObj->property( name );
}

const QString PMPart::getPropertyStr( const QString& name )
{
   PMObject* curObj = activeObject( );

   // Ensure that there is an active object
   if( !curObj )
      return PMVariant( ).asString( );

   return curObj->property( name ).asString( );
}

const PMObjectList& PMPart::selectedObjects( )
{
   uint numObjects = m_selectedObjects.count( );
   uint numOrdered = 0;
   bool stop = false;

   PMObject* tmp;
   QPtrStack<PMObject> stack;

   if( !m_sortedListUpToDate )
   {
      m_sortedSelectedObjects.clear( );

      if( numObjects == 1 )
         m_sortedSelectedObjects.append( m_selectedObjects.first( ) );
      else if( numObjects > 1 )
      {
         tmp = m_pScene;
         do
         {
            if( !tmp )
            {
               if( !stack.isEmpty( ) )
               {
                  tmp = stack.pop( );
                  if( tmp == m_pScene )
                     stop = true;
                  else
                     tmp = tmp->nextSibling( );
               }
               else
                  stop = true;
            }
            else if( tmp->isSelected( ) )
            {
               m_sortedSelectedObjects.append( tmp );
               numOrdered++;
               tmp = tmp->nextSibling( );
            }
            else if( tmp->selectedChildren( ) > 0 )
            {
               stack.push( tmp );
               tmp = tmp->firstChild( );
            }
            else
            {
               tmp = tmp->nextSibling( );
            }
         }
         while( !stop && ( numOrdered < numObjects ) );
      }
      m_sortedListUpToDate = true;
   }

   return m_sortedSelectedObjects;
}

int PMPart::whereToInsert( PMObject* obj, const PMObjectList& list )
{
   // if you change this function, change
   // whereToInsert( PMObject* obj, const QStringList& ), too

   int canInsertAsFirstChild = 0;
   int canInsertAsLastChild = 0;
   int canInsertAsSibling = 0;

   int insertAs = 0;
   int insertPossibilities = 0;

   if( !obj->isReadOnly( ) )
   {
      canInsertAsFirstChild = obj->canInsert( list, 0 );
      if( obj->lastChild( ) )
         canInsertAsLastChild = obj->canInsert( list, obj->lastChild( ) );

      if( canInsertAsFirstChild > 0 )
      {
         // some objects can be inserted as child
         insertAs |= PMInsertPopup::PMIFirstChild;
         insertPossibilities++;
      }
      if( canInsertAsLastChild > 0 )
      {
         insertAs |= PMInsertPopup::PMILastChild;
         insertPossibilities++;
      }
   }

   if( obj->parent( ) )
   {
      PMObject* p = obj->parent( );
      if( !p->isReadOnly( ) )
      {
         canInsertAsSibling = p->canInsert( list, obj );
         if( canInsertAsSibling > 0 )
         {
            // some objects can be inserted as siblings
            insertAs |= PMInsertPopup::PMISibling;
            insertPossibilities++;
         }
      }
   }

   if( insertPossibilities > 1 )
   {
      int count = ( int ) list.count( );
      // more than one possibilities, ask user
      insertAs = PMInsertPopup::choosePlace(
         widget( ), count > 1, insertAs,
         canInsertAsFirstChild == count,
         canInsertAsLastChild == count,
         canInsertAsSibling == count );
   }
   else if( insertPossibilities == 0 )
      insertAs = PMInsertPopup::PMIFirstChild;
   return insertAs;
}

int PMPart::whereToInsert( PMObject* obj, const QStringList& list )
{
   // if you change this function, change
   // whereToInsert( PMObject* obj, const PMObjectList ), too

   int canInsertAsFirstChild = 0;
   int canInsertAsLastChild = 0;
   int canInsertAsSibling = 0;

   int insertAs = 0;
   int insertPossibilities = 0;

   if( !obj->isReadOnly( ) )
   {
      canInsertAsFirstChild = obj->canInsert( list, 0 );
      if( obj->lastChild( ) )
         canInsertAsLastChild = obj->canInsert( list, obj->lastChild( ) );

      if( canInsertAsFirstChild > 0 )
      {
         // some objects can be inserted as child
         insertAs |= PMInsertPopup::PMIFirstChild;
         insertPossibilities++;
      }
      if( canInsertAsLastChild > 0 )
      {
         insertAs |= PMInsertPopup::PMILastChild;
         insertPossibilities++;
      }
   }

   if( obj->parent( ) )
   {
      PMObject* p = obj->parent( );
      if( !p->isReadOnly( ) )
      {
         canInsertAsSibling = p->canInsert( list, obj );
         if( canInsertAsSibling > 0 )
         {
            // some objects can be inserted as siblings
            insertAs |= PMInsertPopup::PMISibling;
            insertPossibilities++;
         }
      }
   }

   if( insertPossibilities > 1 )
   {
      int count = ( int ) list.count( );
      // more than one possibilities, ask user
      insertAs = PMInsertPopup::choosePlace(
         widget( ), count > 1, insertAs,
         canInsertAsFirstChild == count,
         canInsertAsLastChild == count,
         canInsertAsSibling == count );
   }
   else if( insertPossibilities == 0 )
      insertAs = PMInsertPopup::PMIFirstChild;
   return insertAs;
}

int PMPart::whereToInsert( PMObject* obj )
{
   int insertAs = 0;
   int insertPossibilities = 0;

   if( obj->parent( ) )
   {
      insertAs |= PMInsertPopup::PMISibling;
      insertPossibilities++;
   }
   if( obj->isA( "CompositeObject" ) )
   {
      insertAs |= PMInsertPopup::PMIFirstChild;
      insertPossibilities++;
      if( obj->firstChild( ) )
      {
         insertAs |= PMInsertPopup::PMILastChild;
         insertPossibilities++;
      }
   }
   if( insertAs && ( insertPossibilities > 1 ) )
      insertAs = PMInsertPopup::choosePlace( widget( ), true, insertAs );

   return insertAs;
}

void PMPart::slotFileImport( )
{
   QString fileName;
   PMIOFormat* selectedFormat = 0;

   fileName = PMFileDialog::getImportFileName( 0, this, selectedFormat );

   if( !fileName.isEmpty( ) && selectedFormat )
   {
      QFile file( fileName );
      if( file.open( IO_ReadOnly ) )
      {
         PMParser* newParser = selectedFormat->newParser( this, &file );
         if( newParser )
         {
            if( m_pActiveObject )
               insertFromParser( i18n( "Import %1" ).arg( selectedFormat->description( ) ),
                                 newParser, m_pActiveObject );
            else
               insertFromParser( i18n( "Import %1" ).arg( selectedFormat->description( ) ),
                                 newParser, m_pScene );
            delete newParser;
         }
      }
      else
      {
         KMessageBox::error( 0, tr( "Couldn't open the selected file\n"
                                    "Permission denied!" ) );
      }
   }
}

void PMPart::slotFileExport( )
{
	emit aboutToSave( );

   QString fileName, filter;
   PMIOFormat* selectedFormat = 0;

   fileName = PMFileDialog::getExportFileName( 0, this, selectedFormat, filter );

   if( !fileName.isEmpty( ) && selectedFormat )
   {
      QByteArray baData;
      QBuffer buffer( baData );
      buffer.open( IO_WriteOnly );

      PMSerializer* newSer = selectedFormat->newSerializer( &buffer );
      if( newSer )
      {
         newSer->serialize( m_pScene );
         newSer->close( );
         bool success = !( newSer->warnings( ) || newSer->errors( ) );

         if( !success )
         {
            // there were errors, display them
            PMErrorDialog dlg( newSer->messages( ), newSer->errorFlags( ) );
            // still try to export?
            success = ( dlg.exec( ) == QDialog::Accepted );
         }
         if( success )
         {
            QFileInfo info( fileName );
            if( info.extension( ).isEmpty( ) )
               fileName += filter.right( filter.length( ) - 1 ); // remove '*'

            QFile file( fileName );
            if( file.open( IO_WriteOnly ) )
            {
               file.writeBlock( baData );
               file.close( );
            }
            else
            {
               KMessageBox::error( 0, tr( "Couldn't export to the selected file\n"
                                          "Permission denied!" ) );
            }
         }

         delete newSer;
      }
   }
}

void PMPart::slotEditCut( )
{
   emit setStatusBarText( i18n( "Cutting selection..." ) );

   const PMObjectList& sortedList = selectedObjects( );

   if( sortedList.count( ) > 0 )
   {
      QApplication::clipboard( )->setData( new PMObjectDrag( this, sortedList ) );
      removeSelection( i18n( "Cut" ) );
   }

   emit setStatusBarText( "" );
}

void PMPart::slotEditDelete( )
{
   emit setStatusBarText( i18n( "Deleting selection..." ) );

   removeSelection( i18n( "Delete" ) );

   emit setStatusBarText( "" );
}

void PMPart::slotEditCopy( )
{
   emit setStatusBarText( i18n( "Copying selection to clipboard..." ) );
   const PMObjectList& sortedList = selectedObjects( );

   if( sortedList.count( ) > 0 )
      QApplication::clipboard( )->setData( new PMObjectDrag( this, sortedList ) );

   emit setStatusBarText( "" );
}

bool PMPart::dragMoveSelectionTo( PMObject* obj )
{
   if( obj == 0 )
   {
      return removeSelection( i18n( "Drag" ) );
   }
   else
   {
      const PMObjectList& sortedList = selectedObjects( );
      PMMoveCommand* command = 0;
      int insertAs = whereToInsert( obj, sortedList );

      if( insertAs > 0 )
      {
         PMObject* hlp;
         bool stop;

         switch( insertAs )
         {
            case PMInsertPopup::PMIFirstChild:
               command = new PMMoveCommand( sortedList, obj, 0 );
               break;
            case PMInsertPopup::PMILastChild:
               hlp = obj->lastChild( );
               stop = false;

               while( hlp && !stop )
               {
                  if( hlp->isSelected( ) )
                     hlp = hlp->prevSibling( );
                  else
                     stop = true;
               }
               command = new PMMoveCommand( sortedList, obj, hlp );
               break;
            case PMInsertPopup::PMISibling:
               command = new PMMoveCommand( sortedList, obj->parent( ), obj );
               break;
         }
      }
      if( command )
      {
         command->setText( i18n( "Drag" ) );
         return executeCommand( command );
      }
   }
   return false;
}

bool PMPart::removeSelection( const QString& type )
{
   PMDeleteCommand* cmd = 0;
   const PMObjectList& sortedList = selectedObjects( );

   if( sortedList.count( ) > 0 )
   {
      cmd = new PMDeleteCommand( sortedList );
      cmd->setText( type );
      return executeCommand( cmd );
   }
   return false;
}

bool PMPart::drop( PMObject* obj, QMimeSource* mime )
{
   return pasteOrDrop( i18n( "Drop" ), mime, obj );
}

void PMPart::slotEditPaste( )
{
   emit setStatusBarText( i18n( "Inserting clipboard contents..." ) );

   pasteOrDrop( i18n( "Paste" ), qApp->clipboard( )->data( ),
                m_pActiveObject );

   emit setStatusBarText( "" );
}

bool PMPart::pasteOrDrop( const QString& type, QMimeSource* mime, PMObject* obj )
{
   bool success = false;

   if( mime && obj)
   {
      PMParser* parser = PMObjectDrag::newParser( mime, this );

      if( parser )
         success = insertFromParser( type, parser, obj );
   }
   return success;
}

bool PMPart::insertFromParser( const QString& type, PMParser* parser,
                               PMObject* obj )
{
   PMObjectList list;
   bool success = true;
   int insertAs = 0;
   PMAddCommand* command = 0;

   // try to parse
   if( parser->canQuickParse( ) )
   {
      QStringList types;
      parser->quickParse( types );

      success = !( parser->warnings( ) || parser->errors( ) );

      if( !success )
      {
         // there were errors, display them
         PMErrorDialog dlg( parser->messages( ), parser->errorFlags( ) );
         // still try to insert the correct parsed objects?
         success = ( dlg.exec( ) == QDialog::Accepted );
      }
      if( success && ( types.count( ) > 0 ) )
         insertAs = whereToInsert( obj, types );
   }
   else
      insertAs = whereToInsert( obj );

   if( success && insertAs )
   {
      PMObject* parent = 0;
      PMObject* after = 0;

      switch( insertAs )
      {
         case PMInsertPopup::PMIFirstChild:
            parent = obj;
            after = 0;
            break;
         case PMInsertPopup::PMILastChild:
            parent = obj;
            after = obj->lastChild( );
            break;
         case PMInsertPopup::PMISibling:
            parent = obj->parent( );
            after = obj;
            break;
         default:
            parent = obj;
            after = 0;
            break;
      }

      parser->parse( &list, parent, after );
      success = !( parser->warnings( ) || parser->errors( ) );

      if( !success )
      {
         // there were errors, display them
         PMErrorDialog dlg( parser->messages( ), parser->errorFlags( ) );
         // still try to insert the correct parsed objects?
         success = ( dlg.exec( ) == QDialog::Accepted );
      }

      if( list.count( ) > 0 )
      {
         if( success )
         {
            // parsing was successful
            command = new PMAddCommand( list, parent, after );

            command->setText( type );
            success = executeCommand( command );
         }
         else
         {
            // parsed objects will not be inserted
            // remove all links
            PMObjectListIterator it( list );
            PMDeclare* decl = 0;

            for( ; it.current( ); ++it )
            {
               PMRecursiveObjectIterator rit( it.current( ) );
               for( ; rit.current( ); ++rit )
               {
                  decl = rit.current( )->linkedObject( );
                  if( decl )
                     decl->removeLinkedObject( rit.current( ) );
               }
            }
         }
      }
   }
   if( !command )
   {
      // delete all parsed objects
      list.setAutoDelete( true );
      list.clear( );
   }

   return success && insertAs;
}

void PMPart::slotEditUndo( )
{
   emit setStatusBarText( i18n( "Undo last change..." ) );
   m_pNewSelection = 0;
   m_updateNewObjectActions = false;

   m_commandManager.undo( );

   if( m_pNewSelection )
      slotObjectChanged( m_pNewSelection, PMCNewSelection, this );
   if( !isModified( ) )
      setModified( true );
   if( m_updateNewObjectActions )
      updateNewObjectActions( );

   emit setStatusBarText( "" );
}

void PMPart::slotEditRedo( )
{
   emit setStatusBarText( i18n( "Redo last change..." ) );
   m_pNewSelection = 0;
   m_updateNewObjectActions = false;

   m_commandManager.redo( );
   if( m_pNewSelection )
      slotObjectChanged( m_pNewSelection, PMCNewSelection, this );
   if( !isModified( ) )
      setModified( true );
   if( m_updateNewObjectActions )
      updateNewObjectActions( );

   emit setStatusBarText( "" );
}

bool PMPart::executeCommand( PMCommand* cmd )
{
   m_pNewSelection = 0;
   m_numAddedObjects = 0;
   m_numInsertErrors = 0;
   m_insertErrorDetails.clear( );
   m_updateNewObjectActions = false;

   if( isReadWrite( ) && cmd )
   {
      bool execute = true;
      int flags = cmd->errorFlags( this );

      if( flags )
      {
         PMErrorDialog dlg( cmd->messages( ), flags );
         execute = ( dlg.exec( ) == QDialog::Accepted );
      }

      if( execute )
      {
         m_commandManager.execute( cmd );
         if( m_pNewSelection )
            slotObjectChanged( m_pNewSelection, PMCNewSelection, this );
         if( !isModified( ) )
            setModified( true );

         if( m_numInsertErrors > 0 )
         {
            m_insertErrorDetails.sort( );
            PMInsertErrorDialog dlg( m_numAddedObjects, m_numInsertErrors,
                                     m_insertErrorDetails );
            dlg.exec( );
         }
         if( m_updateNewObjectActions )
            updateNewObjectActions( );

         return true;
      }
   }

   delete cmd;
   return false;
}

void PMPart::slotObjectChanged( PMObject* obj, const int m,
                                QObject* sender )
{
   int mode = m;
   bool selectionChanged = false;
   bool changeControlPoints = false;
   PMObject* oldActive = m_pActiveObject;

   if( mode & PMCNewSelection )
   {
      if( !obj )
      {
         clearSelection( );
         selectionChanged = true;
         m_pActiveObject = 0;
      }
      else
      {
         clearSelection( );
         obj->setSelected( true );
         m_selectedObjects.append( obj );
         selectionChanged = true;
         m_pActiveObject = obj;
      }
   }
   else if( ( mode & PMCSelected ) && !obj->isSelected( ) )
   {
      if( obj->isSelectable( ) )
      {
         if( obj->selectedChildren( ) > 0 )
         {
            QPtrStack<PMObject> stack;
            PMObject* tmp = obj->firstChild( );
            bool stop = false;

            do
            {
               if( !tmp )
               {
                  if( !stack.isEmpty( ) )
                  {
                     tmp = stack.pop( );
                     if( tmp == obj )
                        stop = true;
                     else
                        tmp = tmp->nextSibling( );
                  }
                  else
                     stop = true;
               }
               else if( tmp->isSelected( ) )
               {
                  tmp->setSelected( false );
                  m_selectedObjects.removeRef( tmp );
                  emit objectChanged( tmp, PMCDeselected, this );
                  tmp = tmp->nextSibling( );
               }
               else if( tmp->selectedChildren( ) > 0 )
               {
                  stack.push( tmp );
                  tmp = tmp->firstChild( );
               }
               else
               {
                  tmp = tmp->nextSibling( );
               }
            }
            while( !stop );
         }

         obj->setSelected( true );
         m_selectedObjects.append( obj );
         selectionChanged = true;
         m_sortedListUpToDate = false;
         m_sortedSelectedObjects.clear( );
         m_pActiveObject = 0;
      }
      else
      {
         kdError( PMArea ) << "(PMPart::slotObjectChanged) object is not selectable!" << "\n";
         mode = mode & ( ~( PMCSelected | PMCNewSelection ) );
      }
   }
   else if( mode & PMCDeselected )
   {
      // no problems here
      m_selectedObjects.removeRef( obj );
      obj->setSelected( false );
      m_sortedListUpToDate = false;
      m_sortedSelectedObjects.clear( );
      selectionChanged = true;
      m_pActiveObject = 0;
   }

   if( mode & PMCRemove )
   {
      if( obj->parent( ) )
         if( obj->parent( ) == m_pActiveObject )
            m_updateNewObjectActions = true;
      if( m_pNewSelection == obj )
      {
         if( obj->nextSibling( ) )
            m_pNewSelection = obj->nextSibling( );
         else if( obj->prevSibling( ) )
            m_pNewSelection = obj->nextSibling( );
         else if( obj->parent( ) )
            m_pNewSelection = obj->parent( );
         else
            m_pNewSelection = 0;
      }
      if( m_selectedObjects.containsRef( obj ) )
      {
         m_selectedObjects.removeRef( obj );
         if( m_selectedObjects.isEmpty( ) )
         {
            if( obj->nextSibling( ) )
               m_pNewSelection = obj->nextSibling( );
            else if( obj->prevSibling( ) )
               m_pNewSelection = obj->prevSibling( );
            else if( obj->parent( ) )
               m_pNewSelection = obj->parent( );
            else
               m_pNewSelection = 0;
         }
         m_sortedListUpToDate = false;
         m_sortedSelectedObjects.clear( );
         selectionChanged = true;
      }
      if( m_pActiveObject == obj )
         m_pActiveObject = 0;

      if( obj->isA( "Declare" ) )
      {
         PMDeclare* decl = ( PMDeclare* ) obj;
         m_pSymbolTable->remove( decl->id( ) );
      }

      if( obj->type( ) == "Camera" )
         m_cameras.removeRef( ( PMCamera* ) obj );
   }

   if( mode & PMCAdd )
   {
      if( !( mode & PMCInsertError ) )
      {
         m_pNewSelection = obj;
         if( obj->isA( "Declare" ) )
         {
            PMDeclare* decl = ( PMDeclare* ) obj;
            PMSymbol* s = m_pSymbolTable->find( decl->id( ) );
            if( !s )
               m_pSymbolTable->insert( decl->id( ),
                                       new PMSymbol( decl->id( ), decl ) );
         }
         if( obj->type( ) == "Camera" )
            m_bCameraListUpToDate = false;
      }
      if( obj->parent( ) )
         if( obj->parent( ) == m_pActiveObject )
            m_updateNewObjectActions = true;
      m_numAddedObjects++;
   }

   if( mode & PMCData )
   {
      m_updateNewObjectActions = true;
   }

   if( mode & PMCViewStructure )
   {
      changeControlPoints = true;
   }

   if( mode & PMCInsertError )
   {
      m_numInsertErrors++;
      QString detail;
      detail = obj->description( ) + QString( " " ) + obj->name( );
      m_insertErrorDetails.append( detail );

      if( obj->isA( "Declare" ) )
      {
         PMDeclare* decl = ( PMDeclare* ) obj;
         m_pSymbolTable->remove( decl->id( ) );
      }
   }

   if( selectionChanged )
   {
      m_sortedListUpToDate = false;
      m_sortedSelectedObjects.clear( );

      int c = m_selectedObjects.count( );

      if( m_pScene->isSelected( ) )
         c = m_pScene->countChildren( );

      m_pCopyAction->setEnabled( c > 0 );

      if( isReadWrite( ) )
      {
         m_pCutAction->setEnabled( c > 0 );
         m_pDeleteAction->setEnabled( c > 0 );
         m_pPasteAction->setEnabled( m_pActiveObject && m_canDecode );
         updateNewObjectActions( );
      }
   }

   if( ( oldActive != m_pActiveObject ) || changeControlPoints )
   {
      updateControlPoints( oldActive );
      emit objectChanged( m_pActiveObject, PMCNewControlPoints, this );
      mode |= ( PMCNewControlPoints | PMCControlPointSelection );
   }

   emit objectChanged( obj, mode, sender );
}

void PMPart::slotIDChanged( PMObject* obj, const QString& oldID )
{
   if( obj->isA( "Declare" ) )
   {
      PMDeclare* d = ( PMDeclare* ) obj;
      PMSymbol* s = m_pSymbolTable->find( oldID );
      if( s )
      {
         if( s->type( ) == PMSymbol::Object )
         {
            if( s->object( ) == obj )
            {
               m_pSymbolTable->take( oldID );
               s->setId( d->id( ) );
               m_pSymbolTable->insert( s->id( ), s );
            }
            else
               kdError( PMArea ) << "PMPart::slotIDChanged: Symbol "
                                 << oldID << " points to wrong object.\n";
         }
         else
            kdError( PMArea ) << "PMPart::slotIDChanged: Symbol "
                              << oldID << " has wrong type.\n";
      }
      else
         kdError( PMArea ) << "PMPart::slotIDChanged: Symbol "
                           << oldID << " not found.\n";
   }
}

void PMPart::slotNewObject( PMObject* newObject, int insertAs )
{
   PMObjectList list;
   list.append( newObject );
   PMCommand* command = 0;

   if( m_pActiveObject )
   {
      // If no position was specified ask the user
      if( insertAs <= 0 )
         insertAs = whereToInsert( m_pActiveObject, list );
      // If either through a parameter or by user action a position was selected
      if( insertAs > 0 )
      {
         // insert the object in the position indicated
         switch( insertAs )
         {
            case PMInsertPopup::PMIFirstChild:
               command = new PMAddCommand( list, m_pActiveObject, 0 );
               break;
            case PMInsertPopup::PMILastChild:
               command = new PMAddCommand( list, m_pActiveObject,
                                           m_pActiveObject->lastChild( ) );
               break;
            case PMInsertPopup::PMISibling:
               command = new PMAddCommand( list,
                                           m_pActiveObject->parent( ),
                                           m_pActiveObject );
               break;
            default:
               command = new PMAddCommand( list, m_pActiveObject, 0 );
               break;
         }
         executeCommand( command );
      }
      else
      {
         list.clear( );
         delete newObject;
      }
   }
   else
   {
      list.clear( );
      delete newObject;
   }
}

void PMPart::slotNewObject( const QString& type )
{
   PMObject* newObject = m_pPrototypeManager->newObject( type );
   if( newObject )
      slotNewObject( newObject );
}

void PMPart::slotNewObject( const QString& type, const QString& pos )
{
   PMObject* newObject = m_pPrototypeManager->newObject( type );
   if( newObject )
   {
      if( pos == "FirstChild" )
         slotNewObject( newObject, PMInsertPopup::PMIFirstChild );
      else if( pos == "LastChild" )
         slotNewObject( newObject, PMInsertPopup::PMILastChild );
      else if( pos == "Sibling" )
         slotNewObject( newObject, PMInsertPopup::PMISibling );
      else
         slotNewObject( newObject );
   }
}

QStringList PMPart::getObjectTypes( )
{
   QStringList result;
   QPtrListIterator<PMMetaObject> it = m_pPrototypeManager->prototypeIterator( );

   for( ; it.current( ); ++it )
   {
      result.append( it.current( )->className( ) );
   }
   return result;
}

void PMPart::slotNewTransformedObject( PMObject* o )
{
   if( o )
   {
      if( o->canInsert( QString( "Scale" ), o->lastChild( ) ) )
         o->appendChild( new PMScale( this ) );
      if( o->canInsert( QString( "Rotate" ), o->lastChild( ) ) )
         o->appendChild( new PMRotate( this ) );
      if( o->canInsert( QString( "Translate" ), o->lastChild( ) ) )
         o->appendChild( new PMTranslate( this ) );
      slotNewObject( o );
   }
}

void PMPart::slotNewGlobalSettings( )
{
   slotNewObject( new PMGlobalSettings( this ) );
}

void PMPart::slotNewSkySphere( )
{
   slotNewObject( new PMSkySphere( this ) );
}

void PMPart::slotNewRainbow( )
{
   slotNewObject( new PMRainbow( this ) );
}

void PMPart::slotNewFog( )
{
   slotNewObject( new PMFog( this ) );
}

void PMPart::slotNewInterior( )
{
   slotNewObject( new PMInterior( this ) );
}

void PMPart::slotNewMedia( )
{
   slotNewObject( new PMMedia( this ) );
}

void PMPart::slotNewDensity( )
{
   slotNewObject( new PMDensity( this ) );
}

void PMPart::slotNewMaterial( )
{
   slotNewObject( new PMMaterial( this ) );
}

void PMPart::slotNewBox( )
{
   slotNewTransformedObject( new PMBox( this ) );
}

void PMPart::slotNewSphere( )
{
   slotNewTransformedObject( new PMSphere( this ) );
}

void PMPart::slotNewCylinder( )
{
   slotNewTransformedObject( new PMCylinder( this ) );
}

void PMPart::slotNewPlane( )
{
   slotNewTransformedObject( new PMPlane( this ) );
}

void PMPart::slotNewPolynom( )
{
   slotNewTransformedObject( new PMPolynom( this ) );
}

void PMPart::slotNewCone( )
{
   slotNewTransformedObject( new PMCone( this ) );
}

void PMPart::slotNewTorus( )
{
   slotNewTransformedObject( new PMTorus( this ) );
}

void PMPart::slotNewLathe( )
{
   slotNewTransformedObject( new PMLathe( this ) );
}

void PMPart::slotNewPrism( )
{
   slotNewTransformedObject( new PMPrism( this ) );
}

void PMPart::slotNewSurfaceOfRevolution( )
{
   slotNewTransformedObject( new PMSurfaceOfRevolution( this ) );
}

void PMPart::slotNewSuperquadricEllipsoid( )
{
   slotNewTransformedObject( new PMSuperquadricEllipsoid( this ) );
}

void PMPart::slotNewJuliaFractal( )
{
   slotNewTransformedObject( new PMJuliaFractal( this ) );
}

void PMPart::slotNewHeightField( )
{
   slotNewTransformedObject( new PMHeightField( this ) );
}

void PMPart::slotNewText( )
{
   slotNewTransformedObject( new PMText( this ) );
}

void PMPart::slotNewBlob( )
{
   slotNewTransformedObject( new PMBlob( this ) );
}

void PMPart::slotNewBlobSphere( )
{
   slotNewObject( new PMBlobSphere( this ) );
}

void PMPart::slotNewBlobCylinder( )
{
   slotNewObject( new PMBlobCylinder( this ) );
}

void PMPart::slotNewDeclare( )
{
   PMDeclare* obj = new PMDeclare( this );
   m_pSymbolTable->findNewID( i18n( "Declare" ), obj );
   slotNewObject( obj );
}

void PMPart::slotNewObjectLink( )
{
   slotNewTransformedObject( new PMObjectLink( this ) );
}

void PMPart::slotNewUnion( )
{
   slotNewObject( new PMCSG( this, PMCSG::CSGUnion ) );
}

void PMPart::slotNewDifference( )
{
   slotNewObject( new PMCSG( this, PMCSG::CSGDifference ) );
}

void PMPart::slotNewIntersection( )
{
   slotNewObject( new PMCSG( this, PMCSG::CSGIntersection ) );
}

void PMPart::slotNewMerge( )
{
   slotNewObject( new PMCSG( this, PMCSG::CSGMerge ) );
}

void PMPart::slotNewBoundedBy( )
{
   slotNewObject( new PMBoundedBy( this ) );
}

void PMPart::slotNewClippedBy( )
{
   slotNewObject( new PMClippedBy( this ) );
}

void PMPart::slotNewLight( )
{
   slotNewObject( new PMLight( this ) );
}

void PMPart::slotNewLooksLike( )
{
   slotNewObject( new PMLooksLike( this ) );
}

void PMPart::slotNewProjectedThrough( )
{
   slotNewObject( new PMProjectedThrough( this ) );
}

void PMPart::slotNewDisc( )
{
   slotNewTransformedObject( new PMDisc( this ) );
}

void PMPart::slotNewBicubicPatch( )
{
   slotNewTransformedObject( new PMBicubicPatch( this ) );
}

void PMPart::slotNewTriangle( )
{
   slotNewObject( new PMTriangle( this ) );
}

void PMPart::slotNewCamera( )
{
   PMCamera* c = new PMCamera( this );
   c->setAngle( 45.0 );
   c->setLocation( PMVector( 5.0, 5.0, -5.0 ) );
   c->setLookAt( PMVector( 0.0, 0.0, 0.0 ) );

   slotNewObject( c );
}

void PMPart::slotNewTexture( )
{
   slotNewObject( new PMTexture( this ) );
}

void PMPart::slotNewPigment( )
{
   slotNewObject( new PMPigment( this ) );
}

void PMPart::slotNewNormal( )
{
   slotNewObject( new PMNormal( this ) );
}

void PMPart::slotNewSolidColor( )
{
   slotNewObject( new PMSolidColor( this ) );
}

void PMPart::slotNewTextureList( )
{
   slotNewObject( new PMTextureList( this ) );
}

void PMPart::slotNewColorList( )
{
   slotNewObject( new PMColorList( this ) );
}

void PMPart::slotNewPigmentList( )
{
   slotNewObject( new PMPigmentList( this ) );
}

void PMPart::slotNewNormalList( )
{
   slotNewObject( new PMNormalList( this ) );
}

void PMPart::slotNewDensityList( )
{
   slotNewObject( new PMDensityList( this ) );
}

void PMPart::slotNewFinish( )
{
   slotNewObject( new PMFinish( this ) );
}

void PMPart::slotNewWarp( )
{
   slotNewObject( new PMWarp( this ) );
}

void PMPart::slotNewImageMap( )
{
   slotNewObject( new PMImageMap( this ) );
}

void PMPart::slotNewPattern( )
{
   slotNewObject( new PMPattern( this ) );
}

void PMPart::slotNewBlendMapModifiers( )
{
   slotNewObject( new PMBlendMapModifiers( this ) );
}

void PMPart::slotNewTextureMap( )
{
   slotNewObject( new PMTextureMap( this ) );
}

void PMPart::slotNewMaterialMap( )
{
   slotNewObject( new PMMaterialMap( this ) );
}

void PMPart::slotNewColorMap( )
{
   slotNewObject( new PMColorMap( this ) );
}

void PMPart::slotNewPigmentMap( )
{
   slotNewObject( new PMPigmentMap( this ) );
}

void PMPart::slotNewNormalMap( )
{
   slotNewObject( new PMNormalMap( this ) );
}

void PMPart::slotNewBumpMap( )
{
   slotNewObject( new PMBumpMap( this ) );
}

void PMPart::slotNewSlopeMap( )
{
   slotNewObject( new PMSlopeMap( this ) );
}

void PMPart::slotNewDensityMap( )
{
   slotNewObject( new PMDensityMap( this ) );
}

void PMPart::slotNewSlope( )
{
   slotNewObject( new PMSlope( this ) );
}

void PMPart::slotNewQuickColor( )
{
   slotNewObject( new PMQuickColor( this ) );
}

void PMPart::slotNewTranslate( )
{
   slotNewObject( new PMTranslate( this ) );
}

void PMPart::slotNewScale( )
{
   slotNewObject( new PMScale( this ) );
}

void PMPart::slotNewRotate( )
{
   slotNewObject( new PMRotate( this ) );
}

void PMPart::slotNewMatrix( )
{
   slotNewObject( new PMPovrayMatrix( this ) );
}

void PMPart::slotNewComment( )
{
   slotNewObject( new PMComment( this ) );
}

void PMPart::slotNewRaw( )
{
   slotNewObject( new PMRaw( this ) );
}

// POV-Ray 3.5 objects

void PMPart::slotNewIsoSurface( )
{
   slotNewObject( new PMIsoSurface( this ) );
}

void PMPart::slotNewRadiosity( )
{
   slotNewObject( new PMRadiosity( this ) );
}

void PMPart::slotNewGlobalPhotons( )
{
   slotNewObject( new PMGlobalPhotons( this ) );
}

void PMPart::slotNewPhotons( )
{
   slotNewObject( new PMPhotons( this ) );
}

void PMPart::slotNewLightGroup( )
{
   slotNewObject( new PMLightGroup( this ) );
}

void PMPart::slotNewInteriorTexture( )
{
   slotNewObject( new PMInteriorTexture( this ) );
}

void PMPart::slotNewSphereSweep( )
{
   slotNewObject( new PMSphereSweep( this ) );
}

void PMPart::slotNewMesh( )
{
   slotNewObject( new PMMesh( this ) );
}

void PMPart::slotSearchLibraryObject( )
{
   PMLibraryObjectSearch* aux = new PMLibraryObjectSearch( NULL );
   aux->show( );
}

void PMPart::slotClipboardDataChanged( )
{
   if( isReadWrite( ) )
   {
      m_canDecode = PMObjectDrag::canDecode( qApp->clipboard( )->data( ), this );
      m_pPasteAction->setEnabled( m_canDecode && m_pActiveObject );
   }
   else
      m_pPasteAction->setEnabled( false );
}

void PMPart::clearSelection( )
{
   PMObjectListIterator it( m_selectedObjects );

   if( it.current( ) )
   {
      if( it.current( )->nextSibling( ) )
         m_pNewSelection = it.current( )->nextSibling( );
      else if( it.current( )->prevSibling( ) )
         m_pNewSelection = it.current( )->prevSibling( );
      else if( it.current( )->parent( ) )
         m_pNewSelection = it.current( )->parent( );

      for( ; it.current( ); ++it )
      {
         it.current( )->setSelected( false );
         if( m_pNewSelection == it.current( ) )
         {
            if( it.current( )->nextSibling( ) )
               m_pNewSelection = it.current( )->nextSibling( );
            else if( it.current( )->prevSibling( ) )
               m_pNewSelection = it.current( )->prevSibling( );
            else if( it.current( )->parent( ) )
               m_pNewSelection = it.current( )->parent( );
         }
      }
   }

   m_selectedObjects.clear( );
   m_sortedListUpToDate = true;
}

bool PMPart::newDocument( )
{
   deleteContents( );
   setModified( false );

   m_pScene = new PMScene( this );

   PMGlobalSettings* gs = new PMGlobalSettings( this );
   gs->setAssumedGamma( 1.5 );
   m_pScene->appendChild( gs );

   PMBox* b = new PMBox( this );
   m_pScene->appendChild( b );
   PMPigment* p = new PMPigment( this );
   b->appendChild( p );
   PMSolidColor* c = new PMSolidColor( this );
   c->setColor( PMColor( 0.3, 1.0, 0.3 ) );
   p->appendChild( c );
   PMScale* s = new PMScale( this );
   b->appendChild( s );
   PMRotate* r = new PMRotate( this );
   b->appendChild( r );
   PMTranslate* t = new PMTranslate( this );
   t->setTranslation( PMVector( 0, 0.5, 0 ) );
   b->appendChild( t );

   PMLight* l = new PMLight( this );
   l->setLocation( PMVector( 4.0, 5.0, -5.0 ) );
   m_pScene->appendChild( l );
   PMCamera* ca = new PMCamera( this );
   ca->setAngle( 45.0 );
   ca->setLocation( PMVector( 5.0, 5.0, -5.0 ) );
   ca->setLookAt( PMVector( 0.0, 0.0, 0.0 ) );
   m_pScene->appendChild( ca );
   m_bCameraListUpToDate = false;

   m_pScene->setReadOnly( !isReadWrite( ) );
   PMRenderMode* mode = new PMRenderMode( );
   mode->setDescription( i18n( "Default" ) );
   m_pScene->renderModes( )->append( mode );

   emit refresh( );
   updateRenderModes( );
   updateVisibilityLevel( );
   slotObjectChanged( m_pScene, PMCNewSelection, this );

   return true;
}

void PMPart::closeDocument( )
{
   m_url = KURL( );
}

void PMPart::deleteContents( )
{
   emit clear( );
   if( isReadWrite( ) )
      m_commandManager.clear( );
   m_selectedObjects.clear( );
   m_sortedSelectedObjects.clear( );
   m_sortedListUpToDate = true;
   m_pActiveObject = 0;
   m_pNewSelection = 0;

   if( m_pScene )
   {
      delete m_pScene;
      m_pScene = 0;
   }
   if( m_pSymbolTable )
      delete m_pSymbolTable;

   m_pSymbolTable = new PMSymbolTable( );
   m_cameras.clear( );
   m_bCameraListUpToDate = true;
}

void PMPart::slotUpdateUndoRedo( const QString& undo, const QString& redo )
{
   if( isReadWrite( ) )
   {
      if( m_pUndoAction )
      {
         if( undo.isNull( ) )
         {
            m_pUndoAction->setText( i18n( "Undo" ) );
            m_pUndoAction->setEnabled( false );
         }
         else
         {
            m_pUndoAction->setText( i18n( "Undo" ) + " " + undo );
            m_pUndoAction->setEnabled( true );
         }
      }
      if( m_pRedoAction )
      {
         if( redo.isNull( ) )
         {
            m_pRedoAction->setText( i18n( "Redo" ) );
            m_pRedoAction->setEnabled( false );
         }
         else
         {
            m_pRedoAction->setText( i18n( "Redo" ) + " " + redo );
            m_pRedoAction->setEnabled( true );
         }
      }
   }
}

void PMPart::setScene( PMScene* scene )
{
   deleteContents( );
   m_pScene = scene;
   emit refresh( );
   slotObjectChanged( m_pScene, PMCNewSelection, this );
}

void PMPart::setModified( )
{
   KParts::ReadWritePart::setModified( );
   emit modified( );
}

void PMPart::setModified( bool m )
{
   KParts::ReadWritePart::setModified( m );
   emit modified( );
}

PMCamera* PMPart::firstCamera( )
{
   if( !m_bCameraListUpToDate )
      updateCameraList( );
   return m_cameras.first( );
}

QPtrListIterator<PMCamera> PMPart::cameras( )
{
   if( !m_bCameraListUpToDate )
      updateCameraList( );
   return QPtrListIterator<PMCamera>( m_cameras );
}

void PMPart::updateCameraList( )
{
   m_cameras.clear( );
   PMObject* obj;
   for( obj = m_pScene->firstChild( ); obj; obj = obj->nextSibling( ) )
      if( obj->type( ) == "Camera" )
         m_cameras.append( ( PMCamera* ) obj );
   m_bCameraListUpToDate = true;
}

void PMPart::slotRender( )
{
   PMRenderMode* m = m_pScene->renderModes( )->current( );
   if( m )
   {
      emit aboutToRender( );

      QByteArray a;
      QBuffer buffer( a );
      buffer.open( IO_WriteOnly );
      PMPovray35Format format;
      PMSerializer* dev = format.newSerializer( &buffer );
      dev->serialize( m_pScene );
      delete dev;

      if( !m_pPovrayWidget )
         m_pPovrayWidget = new PMPovrayWidget( );
      if( m_pPovrayWidget->render( a, *m, url( ) ) )
      {
         m_pPovrayWidget->show( );
         m_pPovrayWidget->raise( );
      }
   }
}

void PMPart::slotRenderSettings( )
{
   PMRenderModesDialog dlg( m_pScene->renderModes( ), widget( ) );
   int result = dlg.exec( );

   if( result == QDialog::Accepted )
   {
      if( isReadWrite( ) )
         setModified( true );
      updateRenderModes( );
   }
}

void PMPart::slotViewRenderWindow( )
{
   if( !m_pPovrayWidget )
      m_pPovrayWidget = new PMPovrayWidget( );
   m_pPovrayWidget->show( );
   m_pPovrayWidget->raise( );
}

void PMPart::slotRenderMode( int index )
{
   PMRenderModeList* list = m_pScene->renderModes( );
   list->at( index );
   emit activeRenderModeChanged( );
}

void PMPart::updateRenderModes( )
{
   if( m_pScene )
   {
      PMRenderModeList* list = m_pScene->renderModes( );
      PMRenderModeListIterator it( *list );

      QComboBox* box = m_pRenderComboAction->combo( );
      if( box )
      {
         bool b = box->signalsBlocked( );
         box->blockSignals( true );
         box->clear( );

         for( ; it.current( ); ++it )
            box->insertItem( it.current( )->description( ) );
         box->setCurrentItem( list->at( ) );
         box->updateGeometry( );

         box->blockSignals( b );
      }
      emit activeRenderModeChanged( );
   }
}

void PMPart::slotRenderModeActionPlugged( )
{
   updateRenderModes( );
//   connect( m_pRenderComboAction->combo( ), SIGNAL( activated( int ) ),
//            SLOT( slotRenderMode( int ) ) );
}

void PMPart::slotVisibilityLevelChanged( int l )
{
   if( m_pScene->visibilityLevel( ) != l )
   {
      m_pScene->setVisibilityLevel( l );
      if( isReadWrite( ) )
         setModified( true );
      emit objectChanged( m_pScene, PMCViewStructure, this );
   }
}

void PMPart::slotVisibilityActionPlugged( )
{
   if( m_pVisibilityLevelAction )
   {
      QSpinBox* box = m_pVisibilityLevelAction->spinBox( );
      if( box )
      {
         box->setMinValue( -1000 );
         box->setMaxValue( 1000 );
         updateVisibilityLevel( );
      }
   }
}

void PMPart::updateVisibilityLevel( )
{
   if( m_pVisibilityLevelAction )
   {
      QSpinBox* box = m_pVisibilityLevelAction->spinBox( );
      if( box && m_pScene )
      {
         bool sb = box->signalsBlocked( );
         box->blockSignals( true );
         box->setValue( m_pScene->visibilityLevel( ) );
         box->blockSignals( sb );
      }
   }
}

void PMPart::slotGlobalDetailLevelChanged( int level )
{
   PMDetailObject::setGlobalDetailLevel( level + 1 );
   emit objectChanged( m_pScene, PMCViewStructure, this );
}

void PMPart::updateControlPoints( PMObject* oldActive )
{
   PMControlPointList newCPs;

   if( m_pActiveObject )
   {
      m_pActiveObject->controlPoints( newCPs );

      if( m_pActiveObject == oldActive )
      {
         // check if the control points are the same
         bool same = true;
         PMControlPointListIterator oit( m_controlPoints );
         PMControlPointListIterator nit( newCPs );
         while( same && oit.current( ) && nit.current( ) )
         {
            if( oit.current( )->id( ) != nit.current( )->id( ) )
               same = false;
            ++oit;
            ++nit;
         }
         if( oit.current( ) || nit.current( ) )
            same = false;
         if( same )
         {
            // set the old selection
            oit.toFirst( );
            nit.toFirst( );
            while( oit.current( ) && nit.current( ) )
            {
               nit.current( )->setSelected( oit.current( )->selected( ) );
               ++oit;
               ++nit;
            }
         }
      }
   }

   m_controlPoints.clear( );
   m_controlPoints = newCPs;

}

void PMPart::slotAboutToSave( )
{
	emit aboutToSave( );
}

#include "pmpart.moc"
