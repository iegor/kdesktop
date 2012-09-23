//-*-C++-*-
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


#ifndef PMSHELL_H
#define PMSHELL_H

#include "pmdockwidget.h"

#include "pmpart.h"
#include "pmglview.h"
#include "version.h"

#include <qptrlist.h>

//class KHelpMenu;
class KStatusBar;
class KListAction;
class KToggleAction;
class KSelectAction;
class KRecentFilesAction;
class KActionMenu;
class PMViewOptions;

/**
 * Main view for KPovModeler
 */
class PMShell : public PMDockMainWindow
{
   Q_OBJECT

public:
   /**
    * Standard constructor
    */
   PMShell( const KURL& = KURL( ) );
   /**
    * Destructor
    */
   virtual ~PMShell( );

   /**
    * Creates the actions
    */
   void setupActions( );
   /**
    * Creates the docking views
    */
   void setupView( );
   /**
    * Updates the gui (menus and toolbars)
    */
   void updateGUI( );

   /**
    * Checks if a file with that name exists and asks if
    * it should be overwritten.
    *
    * Returns true if the files is not a local file, the file does'n exist
    * or the file should be overwritten.
    */
   static bool overwriteURL( const KURL& u );

public slots:
   void openURL( const KURL& );
   void slotOpenRecent( const KURL& );
   void slotOpenRecentTimer( );

   void slotFileNew( );
   void slotFileOpen( );
   void slotFileSave( );
   void slotFileSaveAs( );
   void slotFileRevert( );
   void slotFilePrint( );
   void slotFileNewWindow( );
   void slotFileClose( );

   void slotShowToolbar( );
   void slotShowStatusbar( );
   void slotShowPath( );
   void slotConfigureKeys( );
   void slotConfigureToolbars( );
   void slotSettings( );
   void slotNewToolbarConfig( );

   void slotNewGraphicalView( PMGLView::PMViewType );
   void slotNewTopView( );
   void slotNewBottomView( );
   void slotNewLeftView( );
   void slotNewRightView( );
   void slotNewFrontView( );
   void slotNewBackView( );
   void slotNewCameraView( );
   void slotNewTreeView( );
   void slotNewDialogView( );
   void slotNewLibraryBrowserView( );

   void saveOptions( );
   void restoreOptions( );

   void setCaption( const QString& caption = QString::null );
   void statusMsg( const QString& text = QString::null );
   void slotControlPointMsg( const QString& msg = QString::null );

   void slotModified( );
   void slotDockWidgetClosed( );
   void slotDeleteClosedObjects( );
   void slotSelectedLayout( int id );
   void slotSaveViewLayout( );
   void slotViewsMenuAboutToShow( );

protected:
   virtual bool queryClose( );
	virtual void showEvent( QShowEvent* );
	void saveAs( );

public:
   /**
    * Creates a dock widget of view type t with custom config c.
    *
    * If initPosition is true, the widget is docked to the desktop.
    */
   PMDockWidget* createView( const QString& t, PMViewOptions* c = 0,
                             bool initPosition = true );
   //PMDockWidget* createTreeView( );
   //PMDockWidget* createDialogView( );
   //PMDockWidget* create3DView( PMGLView::PMViewType );

private:
   KRecentFilesAction* m_pRecent;
   KToggleAction* m_pToolbarAction;
   KToggleAction* m_pStatusbarAction;
   KToggleAction* m_pPathAction;

   KAction* m_pNewTreeViewAction;
   KAction* m_pNewDialogViewAction;
   KAction* m_pNewTopViewAction;
   KAction* m_pNewBottomViewAction;
   KAction* m_pNewLeftViewAction;
   KAction* m_pNewRightViewAction;
   KAction* m_pNewFrontViewAction;
   KAction* m_pNewBackViewAction;
   KAction* m_pNewCameraViewAction;
   KAction* m_pNewLibraryBrowserAction;

   KActionMenu* m_pViewLayoutsAction;
   KAction* m_pSaveViewLayoutAction;

   KStatusBar* m_pStatusBar;
   PMPart* m_pPart;
   KURL m_openRecentURL;

   QPtrList<QObject> m_objectsToDelete;
   int m_viewNumber;
};

#endif
