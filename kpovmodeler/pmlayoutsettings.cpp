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

#include "pmlayoutsettings.h"

#include "pmlineedits.h"
#include "pmviewfactory.h"
#include "pmdebug.h"

#include <qlayout.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <klocale.h>
#include <kmessagebox.h>

PMLayoutSettings::PMLayoutSettings( QWidget* parent, const char* name )
      : PMSettingsDialogPage( parent, name )
{
   QHBoxLayout* hlayout;
   QVBoxLayout* vlayout;
   QVBoxLayout* gvl;
   QGridLayout* grid;
   QGroupBox* gb;
   QGroupBox* gbe;
   QHBoxLayout* ghe;
   QVBoxLayout* gvle;

   vlayout = new QVBoxLayout( this, 0, KDialog::spacingHint( ) );

   hlayout = new QHBoxLayout( vlayout );
   hlayout->addWidget( new QLabel( i18n( "Default view layout:" ), this ) );
   m_pDefaultLayout = new QComboBox( this );
   hlayout->addWidget( m_pDefaultLayout, 1 );
   hlayout->addStretch( 1 );

   gb = new QGroupBox( i18n( "Available View Layouts" ), this );
   vlayout->addWidget( gb );
   gvl = new QVBoxLayout( gb, KDialog::marginHint( ), KDialog::spacingHint( ) );
   gvl->addSpacing( 10 );
   
   grid = new QGridLayout( gvl, 3, 2 );
   m_pViewLayouts = new QListBox( gb );
   connect( m_pViewLayouts, SIGNAL( highlighted( int ) ), 
                            SLOT( slotLayoutSelected( int ) ) );
   grid->addMultiCellWidget( m_pViewLayouts, 0, 2, 0, 0 );
   m_pAddLayout = new QPushButton( i18n( "Add" ), gb );
   connect( m_pAddLayout, SIGNAL( clicked( ) ), SLOT( slotAddLayout( ) ) );
   grid->addWidget( m_pAddLayout, 0, 1 );
   m_pRemoveLayout = new QPushButton( i18n( "Remove" ), gb );
   connect( m_pRemoveLayout, SIGNAL( clicked( ) ), SLOT( slotRemoveLayout( ) ) );
   grid->addWidget( m_pRemoveLayout, 1, 1 );
   grid->setRowStretch( 2, 1 );

   gbe = new QGroupBox( i18n( "View Layout" ), gb );
   gvl->addWidget( gbe );
   gvle = new QVBoxLayout( gbe, KDialog::marginHint( ), KDialog::spacingHint( ) );
   gvle->addSpacing( 10 );
   ghe = new QHBoxLayout( gvle );
   ghe->addWidget( new QLabel( i18n( "Name:" ), gbe ) );
   m_pViewLayoutName = new QLineEdit( gbe );
   connect( m_pViewLayoutName, SIGNAL( textChanged( const QString& ) ), 
                               SLOT( slotLayoutNameChanged( const QString& ) ) );
   ghe->addWidget( m_pViewLayoutName );
   grid = new QGridLayout( gvle, 4, 2 );
   m_pViewEntries = new QListView( gbe );
   m_pViewEntries->setAllColumnsShowFocus( true );
   m_pViewEntries->addColumn( " " );  // This column is for the view entry number 
   m_pViewEntries->addColumn( i18n( "Type" ) );
   m_pViewEntries->addColumn( i18n( "Position" ) );
   m_pViewEntries->setSorting( -1 );
   m_pViewEntries->setMaximumHeight( 150 );
   connect( m_pViewEntries, SIGNAL( selectionChanged( QListViewItem* ) ),
                            SLOT( slotViewEntrySelected( QListViewItem* ) ) );
   grid->addMultiCellWidget( m_pViewEntries, 0, 3, 0, 0 );
   m_pAddEntry = new QPushButton( i18n( "Add" ), gbe );
   connect( m_pAddEntry, SIGNAL( clicked( ) ), SLOT( slotAddViewEntryClicked( ) ) );
   grid->addWidget( m_pAddEntry, 0, 1 );
   m_pRemoveEntry = new QPushButton( i18n( "Remove" ), gbe );
   connect( m_pRemoveEntry, SIGNAL( clicked( ) ), SLOT( slotRemoveViewEntryClicked( ) ) );
   grid->addWidget( m_pRemoveEntry, 1, 1 );
   /* //TODO
   m_pMoveUpEntry = new QPushButton( i18n( "Move Up" ), gbe );
   connect( m_pMoveUpEntry, SIGNAL( clicked( ) ), SLOT( slotMoveUpViewEntryClicked( ) ) );
   grid->addWidget( m_pMoveUpEntry, 2, 1 );
   m_pMoveDownEntry = new QPushButton( i18n( "Move Down" ), gbe );
   connect( m_pMoveDownEntry, SIGNAL( clicked( ) ), SLOT( slotMoveDownViewEntryClicked( ) ) );
   grid->addWidget( m_pMoveDownEntry, 3, 1 );
   */

   QHBoxLayout* ghl = new QHBoxLayout( gvle );
   grid = new QGridLayout( ghl, 7, 4 );
   grid->addWidget( new QLabel( i18n( "Type:" ), gbe ), 0, 0 );
   grid->addWidget( new QLabel( i18n( "Dock position:" ), gbe ), 2, 0 );
   m_pColumnWidthLabel = new QLabel( i18n( "Column width:" ), gbe );
   grid->addWidget( m_pColumnWidthLabel, 3, 0 );
   m_pViewHeightLabel = new QLabel( i18n( "View height:" ), gbe );
   grid->addWidget( m_pViewHeightLabel, 4, 0 );
   m_pViewTypeEdit = new QComboBox( gbe );

   // insert all view types
   const QPtrList<PMViewTypeFactory>& types =
      PMViewFactory::theFactory( )->viewTypes( );
   QPtrListIterator<PMViewTypeFactory> it( types );
   for( ; *it; ++it )
      m_pViewTypeEdit->insertItem( ( *it )->description( ) );
   
   connect( m_pViewTypeEdit, SIGNAL( activated( int ) ),
                             SLOT( slotViewTypeChanged( int ) ) );
   grid->addWidget( m_pViewTypeEdit, 0, 1 );
   m_pDockPositionEdit = new QComboBox( gbe );
   m_pDockPositionEdit->insertItem( i18n( "New Column" ) );
   m_pDockPositionEdit->insertItem( i18n( "Below" ) );
   m_pDockPositionEdit->insertItem( i18n( "Tabbed" ) );
   m_pDockPositionEdit->insertItem( i18n( "Floating" ) );
   connect( m_pDockPositionEdit, SIGNAL( activated( int ) ),
            SLOT( slotDockPositionChanged( int ) ) );
   grid->addWidget( m_pDockPositionEdit, 2, 1 );
   
   m_pColumnWidthEdit = new PMIntEdit( gbe );
   connect( m_pColumnWidthEdit, SIGNAL( textChanged( const QString& ) ),
            SLOT( slotColumnWidthChanged( const QString& ) ) );
   grid->addWidget( m_pColumnWidthEdit, 3, 1 );
   
   m_pViewHeightEdit = new PMIntEdit( gbe );
   connect( m_pViewHeightEdit, SIGNAL( textChanged( const QString& ) ),
            SLOT( slotViewHeightChanged( const QString& ) ) );
   grid->addWidget( m_pViewHeightEdit, 4, 1 );

   m_pFloatingWidthLabel = new QLabel( i18n( "Width:" ), gbe );
   m_pFloatingHeightLabel = new QLabel( i18n( "Height:" ), gbe );
   m_pFloatingPosXLabel = new QLabel( i18n( "Position x:" ), gbe );
   m_pFloatingPosYLabel = new QLabel( QString( "y:" ), gbe );
   grid->addWidget( m_pFloatingWidthLabel, 5, 0 );
   grid->addWidget( m_pFloatingHeightLabel, 5, 2 );
   grid->addWidget( m_pFloatingPosXLabel, 6, 0 );
   grid->addWidget( m_pFloatingPosYLabel, 6, 2 );

   m_pFloatingWidth = new PMIntEdit( gbe );
   connect( m_pFloatingWidth, SIGNAL( textChanged( const QString& ) ),
            SLOT( slotFloatingWidthChanged( const QString& ) ) );
   m_pFloatingHeight = new PMIntEdit( gbe );
   connect( m_pFloatingHeight, SIGNAL( textChanged( const QString& ) ),
            SLOT( slotFloatingHeightChanged( const QString& ) ) );
   m_pFloatingPosX = new PMIntEdit( gbe );
   connect( m_pFloatingPosX, SIGNAL( textChanged( const QString& ) ),
            SLOT( slotFloatingPosXChanged( const QString& ) ) );
   m_pFloatingPosY = new PMIntEdit( gbe );
   connect( m_pFloatingPosY, SIGNAL( textChanged( const QString& ) ),
            SLOT( slotFloatingPosYChanged( const QString& ) ) );
   grid->addWidget( m_pFloatingWidth, 5, 1 );
   grid->addWidget( m_pFloatingHeight, 5, 3 );
   grid->addWidget( m_pFloatingPosX, 6, 1 );
   grid->addWidget( m_pFloatingPosY, 6, 3 );

   // create a holder widget for custom options widgets
   m_pCustomOptionsWidget = 0;
   m_pCustomOptionsHolder = new QWidget( gbe );

   QVBoxLayout* covl = new QVBoxLayout( ghl );
   covl->addWidget( m_pCustomOptionsHolder );
   covl->addStretch( 1 );
   
   ghl->addStretch( 1 );

   vlayout->addStretch( 1 );
}

void PMLayoutSettings::displaySettings( )
{
   PMViewLayoutManager* m = PMViewLayoutManager::theManager( );
   m_viewLayouts = m->layouts( );
   m_currentViewLayout = m_viewLayouts.begin( );
   m_defaultViewLayout = m_viewLayouts.begin( );
   for( ; ( m_defaultViewLayout != m_viewLayouts.end( ) ) &&
          ( *m_defaultViewLayout ).name( ) != m->defaultLayout( );
        ++m_defaultViewLayout );
   displayLayoutList( );
   m_pViewLayouts->setCurrentItem( 0 );
   if( m_pViewLayouts->numRows( ) == 1 )
      m_pRemoveLayout->setEnabled( false );
}

void PMLayoutSettings::displayDefaults( )
{
}

bool PMLayoutSettings::validateData( )
{
   QValueListIterator<PMViewLayout> lit;
   for( lit = m_viewLayouts.begin( ); lit != m_viewLayouts.end( ); ++lit )
   {
      if( ( *lit ).name( ).isEmpty( ) )
      {
         emit showMe( );
         KMessageBox::error( this, i18n( "View layouts may not have empty names." ),
                             i18n( "Error" ) );
         return false;
      }
      QValueListIterator<PMViewLayoutEntry> eit = ( *lit ).begin( );
      if( eit != ( *lit ).end( ) )
      {
         if( ( *eit ).dockPosition( ) != PMDockWidget::DockRight )
         {
            emit showMe( );
            KMessageBox::error( this, i18n( "The docking position of the first view layout entry has to be 'New Column'." ),
                                i18n( "Error" ) );
            return false;
         }
      }
   }
   return true;
}

void PMLayoutSettings::applySettings( )
{
   QValueListIterator<PMViewLayout> lit;
   for( lit = m_viewLayouts.begin( ); lit != m_viewLayouts.end( ); ++lit )
      ( *lit ).normalize( );
   PMViewLayoutManager::theManager( )->setDefaultLayout( m_pDefaultLayout->currentText( ) );
   PMViewLayoutManager::theManager( )->setLayouts( m_viewLayouts );
   PMViewLayoutManager::theManager( )->saveData( );
}

void PMLayoutSettings::displayLayoutList( )
{
   QValueListIterator<PMViewLayout> it;
   
   m_pViewLayouts->clear( );
   m_pDefaultLayout->clear( );
   for( it = m_viewLayouts.begin( ); it != m_viewLayouts.end( ); ++it )
   {
      m_pViewLayouts->insertItem( ( *it ).name( ) );
      m_pDefaultLayout->insertItem( ( *it ).name( ) );
      if( it == m_defaultViewLayout )
         m_pDefaultLayout->setCurrentText( ( *it ).name( ) );
   }
}

void PMLayoutSettings::slotAddLayout( )
{
   QString new_name;
   int i = 1;
   QString str;

   new_name = i18n( "Unnamed" );
   str.setNum( i );
   while( m_pViewLayouts->findItem( new_name, Qt::ExactMatch ) )
   {
      new_name = i18n( "Unnamed" ) + str;
      i++;
      str.setNum( i );
   }

   PMViewLayout l;
   l.setName( new_name );

   m_currentViewLayout++;
   m_viewLayouts.insert( m_currentViewLayout, l );
   displayLayoutList( );
   m_pViewLayouts->setCurrentItem( m_pViewLayouts->findItem( new_name, Qt::ExactMatch ) );
   m_pRemoveLayout->setEnabled( true );
}

void PMLayoutSettings::slotRemoveLayout( )
{
   if( m_currentViewLayout == m_defaultViewLayout )
   {
      m_defaultViewLayout--;
      if( m_defaultViewLayout == m_viewLayouts.end( ) )
      {
         m_defaultViewLayout++;
         m_defaultViewLayout++;
      }
   }
   m_viewLayouts.remove( m_currentViewLayout );
   displayLayoutList( );
   m_pViewLayouts->setCurrentItem( m_pViewLayouts->firstItem( ) );

   if( m_pViewLayouts->numRows( ) == 1 )
      m_pRemoveLayout->setEnabled( false );
}

void PMLayoutSettings::slotLayoutSelected( int index )
{
  int i;
  QString str;
  bool sb;

  m_currentViewLayout = m_viewLayouts.at( index );
  m_currentViewEntry = ( *m_currentViewLayout ).begin( );

  sb = m_pViewLayoutName->signalsBlocked( );
  m_pViewLayoutName->blockSignals( true );
  m_pViewLayoutName->setText( ( *m_currentViewLayout ).name( ) );
  m_pViewLayoutName->blockSignals( sb );
  
  PMViewLayout::iterator it;
  QListViewItem* previous = NULL;
  m_pViewEntries->clear( );
  i = 0;
  for( it = ( *m_currentViewLayout ).begin( );
       it != ( *m_currentViewLayout ).end( ); ++it )
  {
     i++; str.setNum( i );
     previous = new QListViewItem( m_pViewEntries, previous, str,
                                   ( *it ).viewTypeAsString( ),
                                   ( *it ).dockPositionAsString( ) );
     if( i == 1 )
        m_pViewEntries->setSelected( previous, true );
  }
  if( i == 0 )
     slotViewEntrySelected( 0 );
}

void PMLayoutSettings::slotLayoutNameChanged( const QString& text )
{
   int n_item = m_pViewLayouts->currentItem( );
   bool sb = m_pViewLayouts->signalsBlocked( );
   m_pViewLayouts->blockSignals( true );
   m_pViewLayouts->removeItem( n_item );
   m_pViewLayouts->insertItem( text, n_item );
   m_pViewLayouts->setCurrentItem( n_item );
   m_pViewLayouts->blockSignals( sb );

   ( *m_currentViewLayout ).setName( text );
   
   QValueListIterator<PMViewLayout> it;   
   m_pDefaultLayout->clear( );
   for( it = m_viewLayouts.begin( ); it != m_viewLayouts.end( ); ++it )
   {
      m_pDefaultLayout->insertItem( ( *it ).name( ) );
      if( it == m_defaultViewLayout )
         m_pDefaultLayout->setCurrentText( ( *it ).name( ) );
   }
}

void PMLayoutSettings::slotViewEntrySelected( QListViewItem *item )
{
   if( item )
   {
      m_pViewTypeEdit->setEnabled( true );
      m_pDockPositionEdit->setEnabled( true );
      
      int n_item = item->text( 0 ).toInt( ) - 1;

      m_currentViewEntry = ( *m_currentViewLayout ).at( n_item );
      QString vt = ( *m_currentViewEntry ).viewType( );

      // find the view type
      int index = 0;
      bool found = false;
      const QPtrList<PMViewTypeFactory>& types =
         PMViewFactory::theFactory( )->viewTypes( );
      QPtrListIterator<PMViewTypeFactory> it( types );

      for( ; *it && !found; ++it )
      {
         if( ( *it )->viewType( ) == vt )
            found = true;
         else
            index++;
      }
            
      if( !found )
      {
         kdError( PMArea ) << "Unknown view type in PMLayoutSettings::slotViewEntrySelected" << endl;
         m_pViewTypeEdit->setCurrentItem( 0 );
      }
      else
         m_pViewTypeEdit->setCurrentItem( index );
      
      /*
      switch( ( *m_currentViewEntry ).glViewType( ) )
      {
         case PMGLView::PMViewNegY:
            m_pGLViewTypeEdit->setCurrentItem( 0 );
            break;
         case PMGLView::PMViewPosY:
            m_pGLViewTypeEdit->setCurrentItem( 1 );
            break;
         case PMGLView::PMViewPosX:
            m_pGLViewTypeEdit->setCurrentItem( 2 );
            break;
         case PMGLView::PMViewNegX:
            m_pGLViewTypeEdit->setCurrentItem( 3 );
            break;
         case PMGLView::PMViewPosZ:
            m_pGLViewTypeEdit->setCurrentItem( 4 );
            break;
         case PMGLView::PMViewNegZ:
            m_pGLViewTypeEdit->setCurrentItem( 5 );
            break;
         case PMGLView::PMViewCamera:
            m_pGLViewTypeEdit->setCurrentItem( 6 );
            break;
      }
      */
      switch( ( *m_currentViewEntry ).dockPosition( ) )
      {
         case PMDockWidget::DockRight:
            m_pDockPositionEdit->setCurrentItem( 0 );
            m_pColumnWidthLabel->show( );
            m_pColumnWidthEdit->show( );
            m_pViewHeightEdit->show( );
            m_pViewHeightLabel->show( );
            m_pFloatingWidth->hide( );
            m_pFloatingHeight->hide( );
            m_pFloatingPosX->hide( );
            m_pFloatingPosY->hide( );
            m_pFloatingWidthLabel->hide( );
            m_pFloatingHeightLabel->hide( );
            m_pFloatingPosXLabel->hide( );
            m_pFloatingPosYLabel->hide( );
            break;
         case PMDockWidget::DockBottom:
            m_pDockPositionEdit->setCurrentItem( 1 );
            m_pColumnWidthLabel->hide( );
            m_pColumnWidthEdit->hide( );
            m_pViewHeightEdit->show( );
            m_pViewHeightLabel->show( );
            m_pFloatingWidth->hide( );
            m_pFloatingHeight->hide( );
            m_pFloatingPosX->hide( );
            m_pFloatingPosY->hide( );
            m_pFloatingWidthLabel->hide( );
            m_pFloatingHeightLabel->hide( );
            m_pFloatingPosXLabel->hide( );
            m_pFloatingPosYLabel->hide( );
            break;
         case PMDockWidget::DockCenter:
            m_pDockPositionEdit->setCurrentItem( 2 );
            m_pColumnWidthLabel->hide( );
            m_pColumnWidthEdit->hide( );
            m_pViewHeightEdit->hide( );
            m_pViewHeightLabel->hide( );
            m_pFloatingWidth->hide( );
            m_pFloatingHeight->hide( );
            m_pFloatingPosX->hide( );
            m_pFloatingPosY->hide( );
            m_pFloatingWidthLabel->hide( );
            m_pFloatingHeightLabel->hide( );
            m_pFloatingPosXLabel->hide( );
            m_pFloatingPosYLabel->hide( );
            break;
         default:
            m_pDockPositionEdit->setCurrentItem( 3 );
            m_pColumnWidthLabel->hide( );
            m_pColumnWidthEdit->hide( );
            m_pViewHeightEdit->hide( );
            m_pViewHeightLabel->hide( );
            m_pFloatingWidth->show( );
            m_pFloatingHeight->show( );
            m_pFloatingPosX->show( );
            m_pFloatingPosY->show( );
            m_pFloatingWidthLabel->show( );
            m_pFloatingHeightLabel->show( );
            m_pFloatingPosXLabel->show( );
            m_pFloatingPosYLabel->show( );
            break;
      }
      m_pColumnWidthEdit->setValue( ( *m_currentViewEntry ).columnWidth( ) );
      m_pViewHeightEdit->setValue( ( *m_currentViewEntry ).height( ) );
      m_pFloatingWidth->setValue( ( *m_currentViewEntry ).floatingWidth( ) );
      m_pFloatingHeight->setValue( ( *m_currentViewEntry ).floatingHeight( ) );
      m_pFloatingPosX->setValue( ( *m_currentViewEntry ).floatingPositionX( ) );
      m_pFloatingPosY->setValue( ( *m_currentViewEntry ).floatingPositionY( ) );
      m_pViewEntries->triggerUpdate( );
      displayCustomOptions( );
   }
   else
   {
      m_pViewTypeEdit->setEnabled( false );
      m_pDockPositionEdit->setEnabled( false );
      m_pColumnWidthLabel->hide( );
      m_pColumnWidthEdit->hide( );
      m_pViewHeightEdit->hide( );
      m_pViewHeightLabel->hide( );
      m_pFloatingWidth->hide( );
      m_pFloatingHeight->hide( );
      m_pFloatingPosX->hide( );
      m_pFloatingPosY->hide( );
      m_pFloatingWidthLabel->hide( );
      m_pFloatingHeightLabel->hide( );
      m_pFloatingPosXLabel->hide( );
      m_pFloatingPosYLabel->hide( );
   }
}

void PMLayoutSettings::slotViewTypeChanged( int index )
{
   const QPtrList<PMViewTypeFactory>& types =
      PMViewFactory::theFactory( )->viewTypes( );
   QPtrListIterator<PMViewTypeFactory> it( types );
   it += index;
   const PMViewTypeFactory* factory = *it;
   PMViewLayoutEntry& ve = ( *m_currentViewEntry );
   
   if( factory && factory->viewType( ) != ve.viewType( ) )
   {
      ve.setViewType( factory->viewType( ) );
      ve.setCustomOptions( factory->newOptionsInstance( ) );

      QListViewItem* item = m_pViewEntries->currentItem( );
      if( item )
      {
         if( ve.customOptions( ) )
            item->setText( 1, factory->description( ve.customOptions( ) ) );
         else
            item->setText( 1, factory->description( ) );
         displayCustomOptions( );
      }
   }
}

void PMLayoutSettings::slotDockPositionChanged( int index )
{
   switch( index )
   {
      case 0:
         ( *m_currentViewEntry ).setDockPosition( PMDockWidget::DockRight );
         m_pColumnWidthLabel->show( );
         m_pColumnWidthEdit->show( );
         m_pViewHeightEdit->show( );
         m_pViewHeightLabel->show( );
         m_pFloatingWidth->hide( );
         m_pFloatingHeight->hide( );
         m_pFloatingPosX->hide( );
         m_pFloatingPosY->hide( );
         m_pFloatingWidthLabel->hide( );
         m_pFloatingHeightLabel->hide( );
         m_pFloatingPosXLabel->hide( );
         m_pFloatingPosYLabel->hide( );
         break;
      case 1:
         ( *m_currentViewEntry ).setDockPosition( PMDockWidget::DockBottom );
         m_pColumnWidthLabel->hide( );
         m_pColumnWidthEdit->hide( );
         m_pViewHeightEdit->show( );
         m_pViewHeightLabel->show( );
         m_pFloatingWidth->hide( );
         m_pFloatingHeight->hide( );
         m_pFloatingPosX->hide( );
         m_pFloatingPosY->hide( );
         m_pFloatingWidthLabel->hide( );
         m_pFloatingHeightLabel->hide( );
         m_pFloatingPosXLabel->hide( );
         m_pFloatingPosYLabel->hide( );
         break;
      case 2:
         ( *m_currentViewEntry ).setDockPosition( PMDockWidget::DockCenter );
         m_pColumnWidthLabel->hide( );
         m_pColumnWidthEdit->hide( );
         m_pViewHeightEdit->hide( );
         m_pViewHeightLabel->hide( );
         m_pFloatingWidth->hide( );
         m_pFloatingHeight->hide( );
         m_pFloatingPosX->hide( );
         m_pFloatingPosY->hide( );
         m_pFloatingWidthLabel->hide( );
         m_pFloatingHeightLabel->hide( );
         m_pFloatingPosXLabel->hide( );
         m_pFloatingPosYLabel->hide( );
         break;
      case 3:
         ( *m_currentViewEntry ).setDockPosition( PMDockWidget::DockNone );
         m_pColumnWidthLabel->hide( );
         m_pColumnWidthEdit->hide( );
         m_pViewHeightEdit->hide( );
         m_pViewHeightLabel->hide( );
         m_pFloatingWidth->show( );
         m_pFloatingHeight->show( );
         m_pFloatingPosX->show( );
         m_pFloatingPosY->show( );
         m_pFloatingWidthLabel->show( );
         m_pFloatingHeightLabel->show( );
         m_pFloatingPosXLabel->show( );
         m_pFloatingPosYLabel->show( );
         break;
   }
   QListViewItem* item = m_pViewEntries->currentItem( );
   if( item )
      item->setText( 2, ( *m_currentViewEntry ).dockPositionAsString( ) );
}

void PMLayoutSettings::slotViewHeightChanged( const QString& text )
{
   ( *m_currentViewEntry ).setHeight( text.toInt( ) );
}

void PMLayoutSettings::slotColumnWidthChanged( const QString& text )
{
   ( *m_currentViewEntry ).setColumnWidth( text.toInt( ) );
}

void PMLayoutSettings::slotFloatingWidthChanged( const QString& text )
{
   ( *m_currentViewEntry ).setFloatingWidth( text.toInt( ) );
}

void PMLayoutSettings::slotFloatingHeightChanged( const QString& text )
{
   ( *m_currentViewEntry ).setFloatingHeight( text.toInt( ) );
}

void PMLayoutSettings::slotFloatingPosXChanged( const QString& text )
{
   ( *m_currentViewEntry ).setFloatingPositionX( text.toInt( ) );
}

void PMLayoutSettings::slotFloatingPosYChanged( const QString& text )
{
   ( *m_currentViewEntry ).setFloatingPositionY( text.toInt( ) );
}

void PMLayoutSettings::slotAddViewEntryClicked( )
{
   PMViewLayoutEntry p;
   QString str;
   QListViewItem* temp;

   temp = m_pViewEntries->currentItem( );
   if( temp )
   {
      int n_item = temp->text( 0 ).toInt( );
      ( *m_currentViewLayout ).addEntry( p, n_item );
      n_item++;
      str.setNum( n_item );
      QListViewItem* a = new QListViewItem( m_pViewEntries, temp,
                                            str, p.viewTypeAsString( ),
                                            p.dockPositionAsString( ) );
      m_pViewEntries->setSelected( a, true );
      temp = a->nextSibling( );
      while( temp )
      {
         n_item++;
         str.setNum( n_item );
         temp->setText( 0, str );
         temp = temp->nextSibling( );
      }
   } 
   else
   {
      // If there is no selected the list must be empty
      ( *m_currentViewLayout ).addEntry( p );
      str.setNum( 1 );
      QListViewItem* a = new QListViewItem( m_pViewEntries, NULL,
                                            str, p.viewTypeAsString( ),
                                            p.dockPositionAsString( ) );
      m_pViewEntries->setSelected( a, true );
   }
}

void PMLayoutSettings::slotRemoveViewEntryClicked( )
{
   QListViewItem* temp;
   QString str;

   QListViewItem* current = m_pViewEntries->currentItem( );
   if( current )
   {
      int n_item = current->text( 0 ).toInt( ) - 1;
      ( *m_currentViewLayout ).removeEntry( n_item );

      // Save the next selected item in temp, since the current item will
      // be removed.
      temp = current->nextSibling( );
      if( !temp )
         temp = current->itemAbove( );
      else
         n_item++;

      delete current;

      if( temp )
      {
         str.setNum( n_item );
         temp->setText( 0, str );
         m_pViewEntries->setSelected( temp, true );
         n_item++;
         temp = temp->nextSibling( );
      }
      else
         slotViewEntrySelected( 0 );
      while( temp )
      {
         str.setNum( n_item );
         temp->setText( 0, str );
         n_item++;
         temp = temp->nextSibling( );
      }
   }
}

void PMLayoutSettings::slotMoveUpViewEntryClicked( )
{
}

void PMLayoutSettings::slotMoveDownViewEntryClicked( )
{
}

void PMLayoutSettings::displayCustomOptions( )
{
   // delete an old widget
   if( m_pCustomOptionsHolder->layout( ) )
      delete m_pCustomOptionsHolder->layout( );
   if( m_pCustomOptionsWidget )
   {
      delete m_pCustomOptionsWidget;
      m_pCustomOptionsWidget = 0;
   }
   
   if( m_currentViewLayout != m_viewLayouts.end( ) &&
       m_currentViewEntry != ( *m_currentViewLayout ).end( ) &&
       ( *m_currentViewEntry ).customOptions( ) )
   {
      PMViewTypeFactory* vf = PMViewFactory::theFactory( )->viewFactory(
         ( *m_currentViewEntry ).viewType( ) );
      if( vf )
      {
         m_pCustomOptionsWidget =
            vf->newOptionsWidget( m_pCustomOptionsHolder,
                                  ( *m_currentViewEntry ).customOptions( ) );
         if( m_pCustomOptionsWidget )
         {
            connect( m_pCustomOptionsWidget, SIGNAL( viewTypeDescriptionChanged( ) ),
                     SLOT( slotViewTypeDescriptionChanged( ) ) );
            QHBoxLayout* hl = new QHBoxLayout( m_pCustomOptionsHolder,
                                               0, KDialog::spacingHint( ) );
            hl->addWidget( m_pCustomOptionsWidget );
            m_pCustomOptionsWidget->show( );
         }
      }
   }
}

void PMLayoutSettings::slotViewTypeDescriptionChanged( )
{
   PMViewLayoutEntry& ve = *m_currentViewEntry;
   const PMViewTypeFactory* factory =
      PMViewFactory::theFactory( )->viewFactory( ve.viewType( ) );
   
   if( factory )
   {
      QListViewItem* item = m_pViewEntries->currentItem( );
      if( item )
      {
         if( ve.customOptions( ) )
            item->setText( 1, factory->description( ve.customOptions( ) ) );
         else
            item->setText( 1, factory->description( ) );
      }
   }
}

#include "pmlayoutsettings.moc"
