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

#include <klocale.h>
#include <khelpmenu.h>
#include <kstdaction.h>
#include <kaction.h>
#include <kstatusbar.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include <kedittoolbar.h>
#include <kkeydialog.h>
#include <kaboutdialog.h>
#include <kpopupmenu.h>
#include <qtimer.h>

#include "pmshell.h"
#include "pmpart.h"
#include "pmfactory.h"
#include "pmsettingsdialog.h"
#include "pmdefaults.h"
#include "pmdockwidget.h"

#include "pmviewfactory.h"
#include "pmviewbase.h"
#include "pmunknownview.h"
#include "pmviewlayoutmanager.h"

//#define KPM_WITH_OBJECT_LIBRARY

const int c_statusBarInfo = 0;
const int c_statusBarControlPoints = 1;

PMShell::PMShell( const KURL& url )
      : PMDockMainWindow( 0, "mainwindow" )
{
   setPluginLoadingMode( DoNotLoadPlugins );
   setInstance( PMFactory::instance( ), false );

   m_pPart = new PMPart( this, "part", this, "part", true, this );
   m_pPart->setReadWrite( ); // read-write mode
   m_viewNumber = 0;
   m_objectsToDelete.setAutoDelete( true );

   if (!initialGeometrySet())
      resize(800,600);

   setupActions( );

   restoreOptions( );

   setupView( );
   setXMLFile( "kpovmodelershell.rc" );
   createGUI( m_pPart );

   //guiFactory( )->addClient( m_pPart );
   m_pStatusBar = statusBar( );
   m_pStatusBar->insertItem( " ", c_statusBarInfo, 1 );
   m_pStatusBar->insertItem( "" , c_statusBarControlPoints );

   KConfig* config = instance( )->config( );
   config->setGroup( "Appearance" );
   applyMainWindowSettings( config );

   if( !url.isEmpty( ) )
      openURL( url );

   setCaption( url.prettyURL( ) );
   connect( m_pPart, SIGNAL( modified( ) ), SLOT( slotModified( ) ) );
   connect( m_pPart, SIGNAL( controlPointMessage( const QString& ) ),
            SLOT( slotControlPointMsg( const QString& ) ) );
}

PMShell::~PMShell( )
{
}

void PMShell::setupActions( )
{
//   m_helpMenu = new KHelpMenu( this, PMFactory::aboutData( ), true,
//                               actionCollection( ) );

   KStdAction::openNew( this, SLOT( slotFileNew( ) ), actionCollection( ) );
   KStdAction::open( this, SLOT( slotFileOpen( ) ), actionCollection( ) );
   m_pRecent = KStdAction::openRecent( this, SLOT( slotOpenRecent( const KURL& ) ),
                                       actionCollection( ) );
   KStdAction::save( this, SLOT( slotFileSave( ) ), actionCollection( ) );
   KStdAction::saveAs( this, SLOT( slotFileSaveAs( ) ), actionCollection( ) );

   KStdAction::revert( this, SLOT( slotFileRevert( ) ), actionCollection( ) );
   KStdAction::print( this, SLOT( slotFilePrint( ) ), actionCollection( ) );

   KStdAction::close( this, SLOT( slotFileClose( ) ), actionCollection( ) );
   KStdAction::quit( this, SLOT( close( ) ), actionCollection( ) );

   m_pPathAction = new KToggleAction( i18n( "Show &Path" ), 0, this,
                               SLOT( slotShowPath( ) ), actionCollection( ),
                               "options_show_path" );
   m_pPathAction->setCheckedState(i18n("Hide &Path"));

   m_pStatusbarAction = KStdAction::showStatusbar( this, SLOT( slotShowStatusbar( ) ),
                                                   actionCollection( ) );

   KStdAction::saveOptions( this, SLOT( saveOptions( ) ), actionCollection( ) );

   KStdAction::keyBindings( this, SLOT( slotConfigureKeys( ) ),
                            actionCollection( ) );
   KStdAction::configureToolbars( this, SLOT( slotConfigureToolbars( ) ),
                                  actionCollection( ) );
   KStdAction::preferences( this, SLOT( slotSettings( ) ), actionCollection( ) );

   m_pNewTopViewAction = new KAction( i18n( "New Top View" ), 0, this,
                                      SLOT( slotNewTopView( ) ),
                                      actionCollection( ), "view_new_topview" );
   m_pNewBottomViewAction = new KAction( i18n( "New Bottom View" ), 0, this,
                                         SLOT( slotNewBottomView( ) ),
                                         actionCollection( ), "view_new_bottomview" );
   m_pNewLeftViewAction = new KAction( i18n( "New Left View" ), 0, this,
                                       SLOT( slotNewLeftView( ) ),
                                       actionCollection( ), "view_new_leftview" );
   m_pNewRightViewAction = new KAction( i18n( "New Right View" ), 0, this,
                                        SLOT( slotNewRightView( ) ),
                                        actionCollection( ), "view_new_rightview" );
   m_pNewFrontViewAction = new KAction( i18n( "New Front View" ), 0, this,
                                        SLOT( slotNewFrontView( ) ),
                                        actionCollection( ), "view_new_frontview" );
   m_pNewBackViewAction = new KAction( i18n( "New Back View" ), 0, this,
                                       SLOT( slotNewBackView( ) ),
                                       actionCollection( ), "view_new_back_view" );
   m_pNewCameraViewAction = new KAction( i18n( "New Camera View" ), 0, this,
                                         SLOT( slotNewCameraView( ) ),
                                         actionCollection( ), "view_new_cameraview" );

   m_pNewTreeViewAction = new KAction( i18n( "New Object Tree" ), 0, this,
                              SLOT( slotNewTreeView( ) ), actionCollection( ),
                              "view_new_treeview" );
   m_pNewDialogViewAction = new KAction( i18n( "New Properties View" ), 0, this,
                             SLOT( slotNewDialogView( ) ), actionCollection( ),
                             "view_new_dialogview" );

#ifdef KPM_WITH_OBJECT_LIBRARY
   m_pNewLibraryBrowserAction = new KAction( i18n( "New Library Browser" ), 0, this,
                                    SLOT( slotNewLibraryBrowserView( ) ), actionCollection( ),
                                    "view_new_librarybrowser" );
#endif

   // Creating the view layouts menu
   m_pViewLayoutsAction = new KActionMenu( i18n( "View Layouts" ),
                                           actionCollection( ), "view_layouts_menu" );
   KPopupMenu* menu = m_pViewLayoutsAction->popupMenu( );
   connect( menu, SIGNAL( aboutToShow( ) ), SLOT( slotViewsMenuAboutToShow( ) ) );
   PMViewLayoutManager::theManager( )->fillPopupMenu( menu );
   connect( menu, SIGNAL( activated( int ) ), SLOT( slotSelectedLayout( int ) ) );

   m_pSaveViewLayoutAction = new KAction( i18n( "Save View Layout..." ), 0, this,
                                          SLOT( slotSaveViewLayout( ) ),
                                          actionCollection( ), "save_view_layout" );
}

void PMShell::setupView( )
{
   PMViewLayoutManager::theManager( )->displayDefaultLayout( this );
}


PMDockWidget* PMShell::createView( const QString& t, PMViewOptions* o,
                                   bool initPosition )
{
   PMDockWidget* dock = 0;
   PMViewBase* contents = 0;

   PMViewTypeFactory* factory =
      PMViewFactory::theFactory( )->viewFactory( t );

   m_viewNumber++;
   QString name = QString( "View (%1)" ).arg( m_viewNumber );

   if( factory )
   {
      QString desc;
      // Create the appropriate dock widget
      if( o )
         desc = factory->description( o );
      else
         desc = factory->description( );

      dock = createDockWidget( name, SmallIcon( factory->iconName( ) ), 0L,
                               desc, desc );
      contents = factory->newInstance( dock, m_pPart );
      if( o )
         contents->restoreViewConfig( o );
   }
   else
   {
      // unknown view type
      dock = createDockWidget( name, SmallIcon( "unknown" ), 0L,
                               i18n( "Unknown" ), i18n( "Unknown" ) );
      contents = new PMUnknownView( t, dock );
   }

   dock->setWidget( contents );
   connect( dock, SIGNAL( headerCloseButtonClicked( ) ),
            SLOT( slotDockWidgetClosed( ) ) );

   if( initPosition )
   {
      dock->resize( 300, 400 );
      dock->manualDock( 0, PMDockWidget::DockDesktop, 50,
                        mapToGlobal( QPoint( 50, 50 ) ) );
   }
   return dock;
}

/*
PMDockWidget* PMShell::createTreeView( )
{
   PMDockWidget* dock = 0;
   m_numTreeViews++;
   QString name = QString( "Object Tree (%1)" ).arg( m_numTreeViews );
   dock = createDockWidget( name, SmallIcon( "pmtreeview" ),
                            0L, i18n( "Object Tree" ), i18n( "Object Tree" ) );
   dock->setDockSite( PMDockWidget::DockFullSite );
   PMTreeView* tv = new PMTreeView( m_pPart, dock );
   dock->setWidget( tv );

   connect( dock, SIGNAL( headerCloseButtonClicked( ) ),
            SLOT( slotDockWidgetClosed( ) ) );

   return dock;
}

PMDockWidget* PMShell::createDialogView( )
{
   PMDockWidget* dock = 0;
   m_numDialogViews++;
   QString name = QString( "Object Properties (%1)" ).arg( m_numDialogViews );
   dock = createDockWidget( name, SmallIcon( "pmdialogview" ),
                            0L, i18n( "Object Properties" ), i18n( "Object Properties" ) );
   dock->setDockSite( PMDockWidget::DockFullSite );
   PMDialogView* dv = new PMDialogView( m_pPart, dock );
   dock->setWidget( dv );

   connect( dock, SIGNAL( headerCloseButtonClicked( ) ),
            SLOT( slotDockWidgetClosed( ) ) );

   return dock;
}

PMDockWidget* PMShell::create3DView( PMGLView::PMViewType t )
{
   PMDockWidget* dock = 0;
   m_numGLViews++;
   QString name = QString( "3D View (%1)" ).arg( m_numGLViews );
   dock = createDockWidget( name, SmallIcon( "pmglview" ),
                            0L, i18n( "3D View" ), i18n( "3D View" ) );
   dock->setDockSite( PMDockWidget::DockFullSite );
   PMGLView* vgl = new PMGLView( m_pPart, t, dock );
   dock->setWidget( vgl );
   connect( vgl, SIGNAL( viewTypeChanged( const QString& ) ),
            dock, SLOT( slotSetCaption( const QString& ) ) );
   dock->slotSetCaption( PMGLView::viewTypeAsString( t ) );

   connect( dock, SIGNAL( headerCloseButtonClicked( ) ),
            SLOT( slotDockWidgetClosed( ) ) );

   return dock;
}
*/

void PMShell::slotNewGraphicalView( PMGLView::PMViewType t )
{
   PMGLViewOptions* o = new PMGLViewOptions( t );
   createView( "glview", o );
   delete o;
}

void PMShell::slotNewTopView( )
{
   slotNewGraphicalView( PMGLView::PMViewNegY );
}

void PMShell::slotNewBottomView( )
{
   slotNewGraphicalView( PMGLView::PMViewPosY );
}

void PMShell::slotNewLeftView( )
{
   slotNewGraphicalView( PMGLView::PMViewPosX );
}

void PMShell::slotNewRightView( )
{
   slotNewGraphicalView( PMGLView::PMViewNegX );
}

void PMShell::slotNewFrontView( )
{
   slotNewGraphicalView( PMGLView::PMViewPosZ );
}

void PMShell::slotNewBackView( )
{
   slotNewGraphicalView( PMGLView::PMViewNegZ );
}

void PMShell::slotNewCameraView( )
{
   slotNewGraphicalView( PMGLView::PMViewCamera );
}

void PMShell::slotNewDialogView( )
{
   createView( "dialogview" );
}

void PMShell::slotNewTreeView( )
{
   createView( "treeview" );
}

void PMShell::slotNewLibraryBrowserView( )
{
   createView( "librarybrowserview" );
}

void PMShell::slotDockWidgetClosed( )
{
   const QObject* o = sender( );
   if( o && o->inherits( "PMDockWidget" ) )
   {
      if( m_objectsToDelete.containsRef( o ) == 0 )
      {
         m_objectsToDelete.append( o );
         QTimer::singleShot( 0, this, SLOT( slotDeleteClosedObjects( ) ) );
      }
   }
}

void PMShell::slotDeleteClosedObjects( )
{
   m_objectsToDelete.clear( );
}

void PMShell::openURL( const KURL& url )
{
   m_pRecent->addURL( url );

   if( !m_pPart->isModified( ) && m_pPart->url( ).isEmpty( ) )
   {
      m_pPart->openURL( url );
      setCaption( m_pPart->url( ).prettyURL( ) );
   }
   else
   {
      PMShell *shell = new PMShell( );
      shell->show( );
      shell->openURL( url );
   }
}

void PMShell::slotOpenRecent( const KURL& url )
{
   m_openRecentURL = url;
   QTimer::singleShot( 0, this, SLOT( slotOpenRecentTimer( ) ) );
}

void PMShell::slotOpenRecentTimer( )
{
   openURL( m_openRecentURL );
}

void PMShell::slotFileNew( )
{
   if( !m_pPart->isModified( ) && m_pPart->url( ).isEmpty( ) )
   {
      m_pPart->newDocument( );
      setCaption( );
   }
   else
   {
      PMShell *shell = new PMShell( );
      shell->show( );
   }
}

void PMShell::slotFileOpen( )
{
   KURL url = KFileDialog::getOpenURL(
      QString::null, QString( "*.kpm|" ) + i18n( "Povray Modeler Files (*.kpm)" )
      + "\n*|" + i18n( "All Files" ) );

   if( !url.isEmpty( ) )
      openURL( url );
}

void PMShell::slotFileSave( )
{
	m_pPart->slotAboutToSave( );

   if( m_pPart->isModified( ) )
   {
      if( !m_pPart->url( ).isEmpty( ) &&
          m_pPart->isReadWrite( ) )
         m_pPart->saveAs( m_pPart->url( ) );
      else
         saveAs( );
      setCaption( m_pPart->url( ).prettyURL( ) );
   }
   else
      emit statusMsg( i18n( "No changes need to be saved" ) );
}

void PMShell::slotFileSaveAs( )
{
	m_pPart->slotAboutToSave( );
	saveAs( );
}

void PMShell::saveAs( )
{
   KFileDialog dlg( QString::null,
                    QString( "*.kpm|" ) + i18n( "Povray Modeler Files (*.kpm)" ) +
                    QString( "\n*|" ) + i18n( "All Files" ),
                    0, "filedialog", true );
   dlg.setCaption( i18n( "Save As" ) );
   dlg.setOperationMode( KFileDialog::Saving );
   dlg.exec( );

   KURL url = dlg.selectedURL( );

   if( !url.isEmpty( ) )
   {
      if( dlg.currentFilter( ) == QString( "*.kpm" ) )
         if( QFileInfo( url.path( ) ).extension( ).isEmpty( ) )
            url.setPath( url.path( ) + ".kpm" );

      if( overwriteURL( url ) )
      {
         m_pRecent->addURL( url );
         if( m_pPart->saveAs( url ) )
            setCaption( url.prettyURL( ) );
         else
            KMessageBox::sorry( this, i18n( "Couldn't save the file." ) );
      }
   }
}

void PMShell::slotFileRevert( )
{
   KURL url = m_pPart->url( );

   if( !url.isEmpty( ) )
      m_pPart->openURL( url );
//   else
//    slotFileNew( );
}

void PMShell::slotFilePrint( )
{
   //TODO
   //   m_pPart->slotPrint( );
}

void PMShell::slotFileNewWindow( )
{
   PMShell* shell = new PMShell;
   shell->show( );
}

void PMShell::slotFileClose( )
{
   if( m_pPart->closeURL( ) )
   {
      m_pPart->closeDocument( );
      m_pPart->newDocument( );
      setCaption( );
   }
}

void PMShell::slotShowToolbar( )
{
   if( toolBar( )->isVisible ( ) )
      toolBar( )->hide( );
   else
      toolBar( )->show( );
}

void PMShell::slotShowStatusbar( )
{
   if( statusBar( )->isVisible ( ) )
      statusBar( )->hide( );
   else
      statusBar( )->show( );
}

void PMShell::slotShowPath( )
{
   setCaption( m_pPart->url( ).prettyURL( ) );
}

void PMShell::slotConfigureKeys( )
{
   KKeyDialog kd;
   kd.insert( m_pPart->actionCollection( ) );
   kd.insert( actionCollection( ) );
   kd.configure( true );
   //KKeyDialog::configure( actionCollection( ) );
}

void PMShell::slotSettings( )
{
   PMSettingsDialog dlg( m_pPart );
   dlg.exec( );
}

void PMShell::slotConfigureToolbars( )
{
   saveMainWindowSettings( KGlobal::config( ), "Appearance" );
   KEditToolbar dlg( factory( ) );
   connect( &dlg, SIGNAL( newToolbarConfig( ) ),
            this, SLOT( slotNewToolbarConfig( ) ) );
   dlg.exec( );
}

void PMShell::slotNewToolbarConfig( )
{
   createGUI( 0 );
   createShellGUI( false );
   createGUI( m_pPart );
   applyMainWindowSettings( KGlobal::config( ), "Appearance" );
}

void PMShell::updateGUI( )
{
   saveMainWindowSettings( KGlobal::config( ), "Appearance" );
   createGUI( 0 );
   createShellGUI( false );
   createGUI( m_pPart );
   applyMainWindowSettings( KGlobal::config( ), "Appearance" );
}

void PMShell::saveOptions( )
{
   kdDebug( PMArea ) << "Saving configuration" << endl;
   KConfig* config = KGlobal::config( );

   // set group
   config->setGroup( "Appearance" );
   config->writeEntry( "ShowStatusbar", m_pStatusbarAction->isChecked( ) );
   saveMainWindowSettings( config );
   m_pRecent->saveEntries( config );

   if( m_pPart )
      m_pPart->saveConfig( config );

   config->sync( );
}

void PMShell::restoreOptions( )
{
   KConfig* config = instance( )->config( );

   // set group
   config->setGroup( "Appearance" );

   bool showStatusbar = config->readBoolEntry( "ShowStatusbar", true );

   m_pStatusbarAction->blockSignals( true );
   m_pStatusbarAction->setChecked( showStatusbar );
   m_pStatusbarAction->blockSignals( false );

   if( showStatusbar )
      statusBar( )->show( );
   else
      statusBar( )->hide( );

   m_pRecent->loadEntries( config );
}

void PMShell::setCaption( const QString& caption )
{
   QString tmp;

   if( caption.isEmpty( ) )
      tmp = i18n( "unknown" );
   else
   {
      if( !m_pPathAction->isChecked( ) )
         tmp = caption.right( caption.length( ) - caption.findRev( '/' ) - 1 );
      else
         tmp = caption;
   }

   KMainWindow::setCaption( tmp, m_pPart->isModified( ) );
}

void PMShell::statusMsg( const QString& text )
{
   m_pStatusBar->message( text, 5000 );
}

bool PMShell::queryClose( )
{
   saveOptions( );
   return m_pPart->closeURL( );
}

void PMShell::showEvent( QShowEvent* ){
   activateDock( );
}

void PMShell::slotModified( )
{
   setCaption( m_pPart->url( ).prettyURL( ) );
}

void PMShell::slotControlPointMsg( const QString& msg )
{
   if( msg.isEmpty( ) )
      m_pStatusBar->changeItem( msg, c_statusBarControlPoints );
   else
      m_pStatusBar->changeItem( QString( " " ) + msg + QString( " " ),
                                c_statusBarControlPoints );
}

bool PMShell::overwriteURL( const KURL& u )
{
   int query = KMessageBox::Continue;

   if( u.isLocalFile( ) )
   {
      QFileInfo info;
      QString name( u.path( ) );
      info.setFile( name );
      if( info.exists( ) )
         query = KMessageBox::warningContinueCancel( 0, i18n( "A file with this name already exists.\nDo you want to overwrite it?" ), QString::null, i18n("Overwrite") );
   }
   return ( query == KMessageBox::Continue );
}

void PMShell::slotSelectedLayout( int id )
{
   QMenuItem* menu = m_pViewLayoutsAction->popupMenu( )->findItem( id );
   PMViewLayoutManager::theManager( )->displayLayout( menu->text( ), this );
}

void PMShell::slotSaveViewLayout( )
{
   PMSaveViewLayoutDialog dlg( this );
   dlg.exec( );
}

void PMShell::slotViewsMenuAboutToShow( )
{
   KPopupMenu* menu = m_pViewLayoutsAction->popupMenu( );

   PMViewLayoutManager::theManager( )->fillPopupMenu( menu );
}

#include "pmshell.moc"
