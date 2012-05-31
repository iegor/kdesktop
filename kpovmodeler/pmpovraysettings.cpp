/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2003 by Andreas Zehender
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

#include "pmpovraysettings.h"

#include "pmdocumentationmap.h"
#include "pmpovrayrenderwidget.h"
#include "pmdefaults.h"
#include "pmresourcelocator.h"
#include "pmtext.h"

#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kfiledialog.h>

PMPovraySettings::PMPovraySettings( QWidget* parent, const char* name )
      : PMSettingsDialogPage( parent, name )
{
   m_selectionIndex = 0;

   QHBoxLayout* hlayout;
   QVBoxLayout* vlayout;
   QVBoxLayout* gvl;
   QGroupBox* gb;

   vlayout = new QVBoxLayout( this, 0, KDialog::spacingHint( ) );

   gb = new QGroupBox( i18n( "Povray Command" ), this );
   gvl = new QVBoxLayout( gb, KDialog::marginHint( ), KDialog::spacingHint( ) );
   gvl->addSpacing( 10 );
   hlayout = new QHBoxLayout( gvl );
   hlayout->addWidget( new QLabel( i18n( "Command:" ), gb ) );
   m_pPovrayCommand = new QLineEdit( gb );
   hlayout->addWidget( m_pPovrayCommand );
   m_pBrowsePovrayCommand = new QPushButton( gb );
   m_pBrowsePovrayCommand->setPixmap( SmallIcon( "fileopen" ) );
   connect( m_pBrowsePovrayCommand, SIGNAL( clicked( ) ),
            SLOT( slotBrowsePovrayCommand( ) ) );
   hlayout->addWidget( m_pBrowsePovrayCommand );
   vlayout->addWidget( gb );

   gb = new QGroupBox( i18n( "Povray User Documentation" ), this );
   gvl = new QVBoxLayout( gb, KDialog::marginHint( ), KDialog::spacingHint( ) );
   gvl->addSpacing( 10 );
   hlayout = new QHBoxLayout( gvl );
   hlayout->addWidget( new QLabel( i18n( "Path:" ), gb ) );
   m_pDocumentationPath = new QLineEdit( gb );
   hlayout->addWidget( m_pDocumentationPath );
   m_pBrowseDocumentationPath = new QPushButton( gb );
   m_pBrowseDocumentationPath->setPixmap( SmallIcon( "fileopen" ) );
   connect( m_pBrowseDocumentationPath, SIGNAL( clicked( ) ),
            SLOT( slotBrowsePovrayDocumentation( ) ) );
   hlayout->addWidget( m_pBrowseDocumentationPath );
   vlayout->addWidget( gb );
   hlayout = new QHBoxLayout( gvl );
   hlayout->addWidget( new QLabel( i18n( "Version:" ), gb ) );
   m_pDocumentationVersion = new QComboBox( false, gb );
   QValueList<QString> versions = PMDocumentationMap::theMap( )->availableVersions( );
   QValueListIterator<QString> it;
   for( it = versions.begin( ); it != versions.end( ); ++it )
      m_pDocumentationVersion->insertItem( *it );
   hlayout->addWidget( m_pDocumentationVersion );
   hlayout->addStretch( );

   gb = new QGroupBox( i18n( "Library Paths" ), this );
   gvl = new QVBoxLayout( gb, KDialog::marginHint( ), KDialog::spacingHint( ) );
   gvl->addSpacing( 10 );
   hlayout = new QHBoxLayout( gvl );
   m_pLibraryPaths = new QListBox( gb );
   connect( m_pLibraryPaths, SIGNAL( highlighted( int ) ),
            SLOT( slotPathSelected( int ) ) );
   hlayout->addWidget( m_pLibraryPaths );

   QVBoxLayout* bl = new QVBoxLayout( hlayout );
   m_pAddLibraryPath = new QPushButton( i18n( "Add..." ), gb );
   connect( m_pAddLibraryPath, SIGNAL( clicked( ) ), SLOT( slotAddPath( ) ) );
   bl->addWidget( m_pAddLibraryPath );
   m_pRemoveLibraryPath = new QPushButton( i18n( "Remove" ), gb );
   connect( m_pRemoveLibraryPath, SIGNAL( clicked( ) ),
            SLOT( slotRemovePath( ) ) );
   bl->addWidget( m_pRemoveLibraryPath );
   m_pChangeLibraryPath = new QPushButton( i18n( "Edit..." ), gb );
   connect( m_pChangeLibraryPath, SIGNAL( clicked( ) ),
            SLOT( slotEditPath( ) ) );
   bl->addWidget( m_pChangeLibraryPath );
   m_pLibraryPathUp = new QPushButton( i18n( "Up" ), gb );
   connect( m_pLibraryPathUp, SIGNAL( clicked( ) ), SLOT( slotPathUp( ) ) );
   bl->addWidget( m_pLibraryPathUp );
   m_pLibraryPathDown = new QPushButton( i18n( "Down" ), gb );
   connect( m_pLibraryPathDown, SIGNAL( clicked( ) ),
            SLOT( slotPathDown( ) ) );
   bl->addWidget( m_pLibraryPathDown );
   bl->addStretch( 1 );
   vlayout->addWidget( gb );

   vlayout->addStretch( 1 );
}

void PMPovraySettings::displaySettings( )
{
   m_pPovrayCommand->setText( PMPovrayRenderWidget::povrayCommand( ) );
   m_pDocumentationPath->setText( PMDocumentationMap::theMap( )->povrayDocumentationPath( ) );
   int c = m_pDocumentationVersion->count( );
   QString s = PMDocumentationMap::theMap( )->documentationVersion( );
   int i;
   for( i = 0; i < c; i++ )
      if( m_pDocumentationVersion->text( i ) == s )
         m_pDocumentationVersion->setCurrentItem( i );

   bool sb = m_pLibraryPaths->signalsBlocked( );
   m_pLibraryPaths->blockSignals( true );
   m_pLibraryPaths->clear( );
   QStringList plist = PMPovrayRenderWidget::povrayLibraryPaths( );
   QStringList::ConstIterator it = plist.begin( );
   m_selectionIndex = -1;
   for( ; it != plist.end( ); ++it )
   {
      m_pLibraryPaths->insertItem( *it );
      m_selectionIndex++;
   }

   m_pRemoveLibraryPath->setEnabled( false );
   m_pChangeLibraryPath->setEnabled( false );
   m_pLibraryPathUp->setEnabled( false );
   m_pLibraryPathDown->setEnabled( false );
   m_pLibraryPaths->blockSignals( sb );
}

void PMPovraySettings::displayDefaults( )
{
   m_pPovrayCommand->setText( c_defaultPovrayCommand );
   m_pDocumentationVersion->setCurrentItem( 0 );
   m_pDocumentationPath->setText( QString::null );
   m_pLibraryPaths->clear();

}

bool PMPovraySettings::validateData( )
{
   return true;
}

void PMPovraySettings::applySettings( )
{
   PMPovrayRenderWidget::setPovrayCommand( m_pPovrayCommand->text( ) );
   PMDocumentationMap::theMap( )->setPovrayDocumentationPath(
      m_pDocumentationPath->text( ) );
   PMDocumentationMap::theMap( )->setDocumentationVersion(
      m_pDocumentationVersion->currentText( ) );
   QStringList plist;
   int num = ( signed ) m_pLibraryPaths->count( );
   int i;
   for( i = 0; i < num; i++ )
      plist.append( m_pLibraryPaths->text( i ) );

   if( PMPovrayRenderWidget::povrayLibraryPaths( ) != plist )
   {
      PMPovrayRenderWidget::setPovrayLibraryPaths( plist );
      PMResourceLocator::clearCache( );
      PMText::povrayLibraryPathsChanged( );
      emit repaintViews( );
   }
}

void PMPovraySettings::slotAddPath( )
{
   if( m_pLibraryPaths->count( ) >= 20 )
      KMessageBox::error( this, i18n( "Povray only supports up to 20 library paths." ) );
   else
   {
      QString path = KFileDialog::getExistingDirectory( QString::null, this );
      if( !path.isEmpty( ) )
      {
#if ( QT_VERSION >= 300 )
         QListBoxItem* item = m_pLibraryPaths->findItem( path, ExactMatch );
#else
         QListBoxItem* item = 0;
#endif
         if( !item )
         {
            m_pLibraryPaths->insertItem( path, m_selectionIndex + 1 );
            m_pLibraryPaths->setCurrentItem( m_selectionIndex + 1 );
         }
         else
            KMessageBox::error( this, i18n( "The list of library paths already contains this path." ) );
      }
   }
}

void PMPovraySettings::slotRemovePath( )
{
   m_pLibraryPaths->removeItem( m_selectionIndex );
   if( ( unsigned ) m_selectionIndex >= m_pLibraryPaths->count( ) )
      m_selectionIndex--;
   m_pLibraryPaths->setCurrentItem( m_selectionIndex );
}

void PMPovraySettings::slotPathUp( )
{
   QListBoxItem* lbi = m_pLibraryPaths->item( m_selectionIndex );
   if( lbi )
   {
      QString text = lbi->text( );
      m_pLibraryPaths->removeItem( m_selectionIndex );
      if( m_selectionIndex > 0 )
         m_selectionIndex--;
      m_pLibraryPaths->insertItem( text, m_selectionIndex );
      m_pLibraryPaths->setCurrentItem( m_selectionIndex );
   }
}

void PMPovraySettings::slotPathDown( )
{
   QListBoxItem* lbi = m_pLibraryPaths->item( m_selectionIndex );
   if( lbi )
   {
      QString text = lbi->text( );
      m_pLibraryPaths->removeItem( m_selectionIndex );
      if( ( unsigned ) m_selectionIndex < m_pLibraryPaths->count( ) )
         m_selectionIndex++;
      m_pLibraryPaths->insertItem( text, m_selectionIndex );
      m_pLibraryPaths->setCurrentItem( m_selectionIndex );
   }
}

void PMPovraySettings::slotEditPath( )
{
   QListBoxItem* lbi = m_pLibraryPaths->item( m_selectionIndex );
   if( lbi )
   {
      QString text = lbi->text( );
      QString path = KFileDialog::getExistingDirectory( text, this );
      if( !path.isEmpty( ) )
      {
#if ( QT_VERSION >= 300 )
         QListBoxItem* item = m_pLibraryPaths->findItem( path, ExactMatch );
#else
         QListBoxItem* item = 0;
#endif
         if( !item )
            m_pLibraryPaths->changeItem( path, m_selectionIndex );
         else if( item != lbi )
            KMessageBox::error( this, i18n( "The list of library paths already contains this path." ) );
      }
   }
}

void PMPovraySettings::slotPathSelected( int index )
{
   m_selectionIndex = index;
   QListBoxItem* lbi = m_pLibraryPaths->item( m_selectionIndex );
   if( lbi )
   {
      m_pRemoveLibraryPath->setEnabled( true );
      m_pChangeLibraryPath->setEnabled( true );
      m_pLibraryPathUp->setEnabled( index > 0 );
      m_pLibraryPathDown->setEnabled( index < ( ( signed ) m_pLibraryPaths->count( ) - 1 ) );
   }
   else
   {
      m_pRemoveLibraryPath->setEnabled( false );
      m_pChangeLibraryPath->setEnabled( false );
      m_pLibraryPathUp->setEnabled( false );
      m_pLibraryPathDown->setEnabled( false );
   }
}

void PMPovraySettings::slotBrowsePovrayCommand( )
{
   QString str = KFileDialog::getOpenFileName( QString::null, QString::null );

   if( !str.isEmpty() )
   {
      m_pPovrayCommand->setText( str );
   }
}

void PMPovraySettings::slotBrowsePovrayDocumentation( )
{
   QString str = KFileDialog::getExistingDirectory( );

   if( !str.isEmpty( ) )
      m_pDocumentationPath->setText( str );
}

#include "pmpovraysettings.moc"
