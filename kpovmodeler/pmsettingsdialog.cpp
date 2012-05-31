/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2003 by Andreas Zehender
    email                : zehender@kde.org
    copyright            : (C) 2002 by Luis Carvalho
    email                : lpassos@mail.telepac.pt
**************************************************************************

**************************************************************************
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation; either version 2 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
**************************************************************************/

#include "pmsettingsdialog.h"

#include "pmpovraysettings.h"
#include "pmcolorsettings.h"
#include "pmgridsettings.h"
#include "pmobjectsettings.h"
#include "pmpreviewsettings.h"
#include "pmlayoutsettings.h"
#include "pmobjectlibrarysettings.h"
#include "pmpluginsettings.h"
#include "pmopenglsettings.h"

#include "pmrendermanager.h"
#include "pmdebug.h"

#include <qvbox.h>
#include <qlistview.h>
#include <klocale.h>
#include <kconfig.h>
#include <kiconloader.h>

//#define KPM_WITH_OBJECT_LIBRARY

PMSettingsDialogPage::PMSettingsDialogPage( QWidget* parent, const char* name )
      : QWidget( parent, name )
{
}

QSize PMSettingsDialog::s_size = QSize( 640, 400 );

PMSettingsDialog::PMSettingsDialog( PMPart* part, QWidget* parent, const char* name )
      : KDialogBase( TreeList, i18n( "Configure" ), Ok | Apply | Cancel | Default, Ok,
                     parent, name )
{
   QStringList sl;
   QWidget* w = 0;
   PMSettingsDialogPage* p = 0;

   m_pPart = part;

   setShowIconsInTreeList( true );

   sl.clear( );
   sl.append( i18n( "Povray" ) );
   w = addVBoxPage( sl, i18n( "Povray Options" ),
                    SmallIcon( "pmconfigurepovray", 22 ) );
   p = new PMPovraySettings( w );
   registerPage( w, p );

   sl.clear( );
   sl.append( i18n( "Graphical View" ) );
   sl.append( i18n( "OpenGL" ) );
   w = addVBoxPage( sl, i18n( "OpenGL Display Settings" ),
                    SmallIcon( "pmconfigureopengl", 22 ) );
   p = new PMOpenGLSettings( w );
   registerPage( w, p );

   sl.clear( );
   sl.append( i18n( "Graphical View" ) );
   setFolderIcon( sl, SmallIcon( "pmconfiguregraphicalview", 22 ) );
   sl.append( i18n( "Colors" ) );
   w = addVBoxPage( sl, i18n( "Color Settings" ),
                    SmallIcon( "pmconfigurecolors", 22 ) );
   p = new PMColorSettings( w );
   registerPage( w, p );

   sl.clear( );
   sl.append( i18n( "Graphical View" ) );
   sl.append( i18n( "Grid" ) );
   w = addVBoxPage( sl, i18n( "Grid Settings" ),
                    SmallIcon( "pmconfiguregrid", 22 ) );
   p = new PMGridSettings( w );
   registerPage( w, p );

   sl.clear( );
   sl.append( i18n( "Graphical View" ) );
   sl.append( i18n( "Objects" ) );
   w = addVBoxPage( sl, i18n( "Display Settings for Objects" ),
                    SmallIcon( "pmconfigureobjects", 22 ) );
   p = new PMObjectSettings( w );
   registerPage( w, p );

   sl.clear( );
   sl.append( i18n( "Properties View" ) );
   setFolderIcon( sl, SmallIcon( "pmconfiguredialogview", 22 ) );
   sl.append( i18n( "Texture Preview" ) );
   w = addVBoxPage( sl, i18n( "Display Settings for Texture Previews" ),
                    SmallIcon( "pmconfiguretexturepreview", 22 ) );
   p = new PMPreviewSettings( w );
   registerPage( w, p );

   sl.clear( );
   sl.append( i18n( "View Layout" ) );
   w = addVBoxPage( sl, i18n( "Display Settings for View Layouts" ),
                    SmallIcon( "pmconfigureviewlayout", 22 ) );
   p = new PMLayoutSettings( w );
   registerPage( w, p );

#ifdef KPM_WITH_OBJECT_LIBRARY
   sl.clear( );
   sl.append( i18n( "Object Libraries" ) );
   w = addVBoxPage( sl, i18n( "Display Settings for Object Libraries" ),
                    SmallIcon( "pmconfigureobjectlibrary", 22 ) );
   p = new PMObjectLibrarySettings( w );
   registerPage( w, p );
#endif

#ifdef KPM_WITH_PLUGINS
   sl.clear( );
   sl.append( i18n( "Plugins" ) );
   w = addVBoxPage( sl, i18n( "Plugin Settings" ) );
   p = new PMPluginSettings( w );
   registerPage( w, p );
#endif

   displaySettings( );

   resize( s_size );
}

void PMSettingsDialog::displaySettings( )
{
   QValueList<PMRegisteredSettingsPage>::const_iterator it;
   for( it = m_pages.begin( ); it != m_pages.end( ); ++it )
      ( *it ).page->displaySettings( );
}

void PMSettingsDialog::slotCancel( )
{
   QDialog::reject( );
}

void PMSettingsDialog::slotApply( )
{
   if( validateData( ) )
      saveSettings( );
}

void PMSettingsDialog::slotOk( )
{
   if( validateData( ) )
   {
      saveSettings( );
      QDialog::accept( );
   }
}

void PMSettingsDialog::slotDefault( )
{
   int currentPage = activePageIndex( );
   PMSettingsDialogPage* page = 0;
   QValueList<PMRegisteredSettingsPage>::const_iterator it;
   for( it = m_pages.begin( ); it != m_pages.end( ) && !page; ++it )
      if( ( *it ).index == currentPage )
         page = ( *it ).page;
   if( page )
      page->displayDefaults( );
}

bool PMSettingsDialog::validateData( )
{
   bool valid = true;
   QValueList<PMRegisteredSettingsPage>::const_iterator it;
   for( it = m_pages.begin( ); it != m_pages.end( ) && valid; ++it )
      valid = ( *it ).page->validateData( );
   return valid;
}

void PMSettingsDialog::saveSettings( )
{
   m_repaint = false;

   QValueList<PMRegisteredSettingsPage>::const_iterator it;
   for( it = m_pages.begin( ); it != m_pages.end( ); ++it )
      ( *it ).page->applySettings( );

   if( m_repaint )
   {
      PMRenderManager* rm = PMRenderManager::theManager( );
      rm->slotRenderingSettingsChanged( );
   }
}

void PMSettingsDialog::saveConfig( KConfig* cfg )
{
   cfg->setGroup( "Appearance" );
   cfg->writeEntry( "SettingsDialogSize", s_size );
}

void PMSettingsDialog::restoreConfig( KConfig* cfg )
{
   cfg->setGroup( "Appearance" );

   QSize defaultSize( 640, 400 );
   s_size = cfg->readSizeEntry( "SettingsDialogSize", &defaultSize );
}

void PMSettingsDialog::resizeEvent( QResizeEvent* ev )
{
   s_size = ev->size( );
}

void PMSettingsDialog::registerPage( QWidget* topPage,
                                     PMSettingsDialogPage* page )
{
   int i = pageIndex( topPage );
   if( i < 0 )
      kdError( PMArea ) << "PMSettingsDialog: Registered settings page"
                        << " not found" << endl;
   else
   {
      m_pages.push_back( PMRegisteredSettingsPage( topPage, page, i ) );
      connect( page, SIGNAL( repaintViews( ) ), SLOT( slotRepaint( ) ) );
      connect( page, SIGNAL( showMe( ) ), SLOT( slotShowPage( ) ) );
   }
}

void PMSettingsDialog::slotRepaint( )
{
   m_repaint = true;
}

void PMSettingsDialog::slotShowPage( )
{
   const QObject* w = sender( );
   if( w )
   {
      int index = findPage( ( const PMSettingsDialogPage* ) w );
      if( index >= 0 )
         showPage( index );
   }
}

int PMSettingsDialog::findPage( const PMSettingsDialogPage* p )
{
   int index = -1;
   QValueList<PMRegisteredSettingsPage>::const_iterator it;
   for( it = m_pages.begin( ); it != m_pages.end( ) && index < 0; ++it )
      if( ( *it ).page == p )
         index = ( *it ).index;
   return index;
}

#include "pmsettingsdialog.moc"
