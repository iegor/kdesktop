/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2002 by Luis Passos Carvalho
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


#include "pmimagemapedit.h"
#include "pmimagemap.h"
#include "pmvectoredit.h"
#include "pmlineedits.h"
#include "pmpalettevalueedit.h"
#include "pmvector.h"

#include <qwidget.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <ktabctl.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdialog.h>
#include <kfiledialog.h>
#include <kiconloader.h>

PMImageMapEdit::PMImageMapEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}

void PMImageMapEdit::createTopWidgets( )
{
   QLabel* lbl;
   QHBoxLayout* hl;
   
   Base::createTopWidgets( );

   hl = new QHBoxLayout( topLayout( ) );
   lbl = new QLabel( i18n( "File type:" ), this );
   m_pImageFileTypeEdit = new QComboBox( this );
   m_pImageFileTypeEdit->insertItem( "gif" );
   m_pImageFileTypeEdit->insertItem( "tga" );
   m_pImageFileTypeEdit->insertItem( "iff" );
   m_pImageFileTypeEdit->insertItem( "ppm" );
   m_pImageFileTypeEdit->insertItem( "pgm" );
   m_pImageFileTypeEdit->insertItem( "png" );
   m_pImageFileTypeEdit->insertItem( "jpeg" );
   m_pImageFileTypeEdit->insertItem( "tiff" );
   m_pImageFileTypeEdit->insertItem( "sys" );
   hl->addWidget( lbl );
   hl->addWidget( m_pImageFileTypeEdit );

   hl = new QHBoxLayout( topLayout( ) );
   lbl = new QLabel( i18n( "File name:" ), this );
   m_pImageFileNameEdit = new QLineEdit( this );
   m_pImageFileNameBrowse = new QPushButton( this );
   m_pImageFileNameBrowse->setPixmap( SmallIcon( "fileopen" ) );
   hl->addWidget( lbl );
   hl->addWidget( m_pImageFileNameEdit );
   hl->addWidget( m_pImageFileNameBrowse );

   hl = new QHBoxLayout( topLayout( ) );
   m_pEnableFilterAllEdit = new QCheckBox( i18n( "Filter all" ), this );
   m_pFilterAllEdit = new PMFloatEdit( this );
   hl->addWidget( m_pEnableFilterAllEdit );
   hl->addWidget( m_pFilterAllEdit );
   hl->addStretch( 1 );

   hl = new QHBoxLayout( topLayout( ) );
   m_pEnableTransmitAllEdit = new QCheckBox( i18n( "Transmit all" ), this );
   m_pTransmitAllEdit = new PMFloatEdit( this );
   hl->addWidget( m_pEnableTransmitAllEdit );
   hl->addWidget( m_pTransmitAllEdit );
   hl->addStretch( 1 );

   m_pOnceEdit = new QCheckBox( i18n( "Once" ), this );
   topLayout( )->addWidget( m_pOnceEdit );

   hl = new QHBoxLayout( topLayout( ) );
   lbl = new QLabel( i18n( "Interpolate:" ), this );
   m_pInterpolateTypeEdit = new QComboBox( this );
   m_pInterpolateTypeEdit->insertItem( i18n( "None" ) );
   m_pInterpolateTypeEdit->insertItem( i18n( "Bilinear" ) );
   m_pInterpolateTypeEdit->insertItem( i18n( "Normalized" ) );
   hl->addWidget( lbl );
   hl->addWidget( m_pInterpolateTypeEdit );

   hl = new QHBoxLayout( topLayout( ) );
   lbl = new QLabel( i18n( "Map type:" ), this );
   m_pMapTypeEdit = new QComboBox( this );
   m_pMapTypeEdit->insertItem( i18n( "Planar" ) );
   m_pMapTypeEdit->insertItem( i18n( "Spherical" ) );
   m_pMapTypeEdit->insertItem( i18n( "Cylindrical" ) );
   m_pMapTypeEdit->insertItem( i18n( "Toroidal" ) );
   hl->addWidget( lbl );
   hl->addWidget( m_pMapTypeEdit );

   connect( m_pImageFileTypeEdit, SIGNAL( activated( int ) ), SLOT( slotImageFileTypeChanged( int ) ) );
   connect( m_pMapTypeEdit, SIGNAL( activated( int ) ), SLOT( slotMapTypeChanged( int ) ) );
   connect( m_pInterpolateTypeEdit, SIGNAL( activated( int ) ), SLOT( slotInterpolateTypeChanged( int ) ) );
   connect( m_pImageFileNameBrowse, SIGNAL( clicked( ) ), SLOT( slotImageFileBrowseClicked( ) ) );
   connect( m_pImageFileNameEdit, SIGNAL( textChanged( const QString& ) ), SLOT( slotImageFileNameChanged( const QString& ) ) );
   connect( m_pEnableFilterAllEdit, SIGNAL( clicked( ) ), SLOT( slotFilterAllClicked( ) ) );
   connect( m_pEnableTransmitAllEdit, SIGNAL( clicked( ) ), SLOT( slotTransmitAllClicked( ) ) );
   connect( m_pFilterAllEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pTransmitAllEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pOnceEdit, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );
}

void PMImageMapEdit::createBottomWidgets( )
{
   topLayout( )->addWidget( new QLabel( i18n( "Indexed filters" ), this ) );
   m_pFiltersWidget = new QWidget( this );
   topLayout( )->addWidget( m_pFiltersWidget );
   topLayout( )->addWidget( new QLabel( i18n( "Indexed transmits" ), this ) );
   m_pTransmitsWidget = new QWidget( this );
   topLayout( )->addWidget( m_pTransmitsWidget );
   
   
   Base::createBottomWidgets( );
}

void PMImageMapEdit::displayObject( PMObject* o )
{
   bool readOnly;

   if( o->isA( "ImageMap" ) )
   {
      m_pDisplayedObject = ( PMImageMap* ) o;
      readOnly = m_pDisplayedObject->isReadOnly( );

      switch( m_pDisplayedObject->bitmapType( ) )
      {
         case PMImageMap::BitmapGif:
            m_pImageFileTypeEdit->setCurrentItem( 0 );
            break;
         case PMImageMap::BitmapTga:
            m_pImageFileTypeEdit->setCurrentItem( 1 );
            break;
         case PMImageMap::BitmapIff:
            m_pImageFileTypeEdit->setCurrentItem( 2 );
            break;
         case PMImageMap::BitmapPpm:
            m_pImageFileTypeEdit->setCurrentItem( 3 );
            break;
         case PMImageMap::BitmapPgm:
            m_pImageFileTypeEdit->setCurrentItem( 4 );
            break;
         case PMImageMap::BitmapPng:
            m_pImageFileTypeEdit->setCurrentItem( 5 );
            break;
         case PMImageMap::BitmapJpeg:
            m_pImageFileTypeEdit->setCurrentItem( 6 );
            break;
         case PMImageMap::BitmapTiff:
            m_pImageFileTypeEdit->setCurrentItem( 7 );
            break;
         case PMImageMap::BitmapSys:
            m_pImageFileTypeEdit->setCurrentItem( 8 );
            break;
      }
      m_pImageFileTypeEdit->setEnabled( !readOnly );

      switch( m_pDisplayedObject->interpolateType( ) )
      {
         case PMImageMap::InterpolateNone:
            m_pInterpolateTypeEdit->setCurrentItem( 0 );
            break;
         case PMImageMap::InterpolateBilinear:
            m_pInterpolateTypeEdit->setCurrentItem( 1);
            break;
         case PMImageMap::InterpolateNormalized:
            m_pInterpolateTypeEdit->setCurrentItem( 2 );
            break;
      }
      m_pInterpolateTypeEdit->setEnabled( !readOnly );

      switch( m_pDisplayedObject->mapType( ) )
      {
         case PMImageMap::MapPlanar:
            m_pMapTypeEdit->setCurrentItem( 0 );
            break;
         case PMImageMap::MapSpherical:
            m_pMapTypeEdit->setCurrentItem( 1 );
            break;
         case PMImageMap::MapCylindrical:
            m_pMapTypeEdit->setCurrentItem( 2 );
            break;
         case PMImageMap::MapToroidal:
            m_pMapTypeEdit->setCurrentItem( 3 );
            break;
      }
      m_pMapTypeEdit->setEnabled( !readOnly );

      m_pImageFileNameEdit->setText( m_pDisplayedObject->bitmapFile( ) );
      m_pImageFileNameEdit->setEnabled( !readOnly );
      m_pOnceEdit->setChecked( m_pDisplayedObject->isOnceEnabled( ) );
      m_pOnceEdit->setEnabled( !readOnly );
      m_pEnableFilterAllEdit->setChecked( m_pDisplayedObject->isFilterAllEnabled( ) );
      m_pEnableFilterAllEdit->setEnabled( !readOnly );
      m_pFilterAllEdit->setValue( m_pDisplayedObject->filterAll( ) );
      m_pFilterAllEdit->setReadOnly( readOnly );
      m_pEnableTransmitAllEdit->setChecked( m_pDisplayedObject->isTransmitAllEnabled( ) );
      m_pEnableTransmitAllEdit->setEnabled( !readOnly );
      m_pTransmitAllEdit->setValue( m_pDisplayedObject->transmitAll( ) );
      m_pTransmitAllEdit->setReadOnly( readOnly );

      displayPaletteEntries( m_pDisplayedObject->filters( ), m_pDisplayedObject->transmits( ) );

      slotFilterAllClicked( );
      slotTransmitAllClicked( );

      Base::displayObject( o );
   }
    
}

void PMImageMapEdit::displayPaletteEntries( const QValueList<PMPaletteValue>& filters,
                                            const QValueList<PMPaletteValue>& transmits )
{
   bool readOnly = m_pDisplayedObject->isReadOnly( );

   int nfilters = ( int ) filters.count( );
   int ntransmits = ( int ) transmits.count( );

   int i;
   PMPaletteValueEdit* edit;
   QPushButton* button;
   QGridLayout* gl;
   QPixmap addButtonPixmap = SmallIcon( "pmaddpoint" );
   QPixmap removeButtonPixmap = SmallIcon( "pmremovepoint" );

   // First let's deal with the filter entries...
   if( m_pFiltersWidget->layout( ) )
      delete m_pFiltersWidget->layout( );

   m_filterEntries.setAutoDelete( true );
   m_filterAddButtons.setAutoDelete( true );
   m_filterRemoveButtons.setAutoDelete( true );
   m_filterEntries.clear( );
   m_filterAddButtons.clear( );
   m_filterRemoveButtons.clear( );
   m_filterEntries.setAutoDelete( false );
   m_filterAddButtons.setAutoDelete( false );
   m_filterRemoveButtons.setAutoDelete( false );

   gl = new QGridLayout( m_pFiltersWidget, nfilters + 1, 3, 0, KDialog::spacingHint( ) );

   button = new QPushButton( m_pFiltersWidget );
   button->setPixmap( addButtonPixmap );
   m_filterAddButtons.append( button );
   connect( button, SIGNAL( clicked( ) ), SLOT( slotAddFilterEntry( ) ) );
   gl->addWidget( button, 0, 1 );
   button->show( );
   button->setEnabled( !readOnly );
   QToolTip::add( button, i18n( "Add new filter" ) );

   for( i = 0; i < nfilters; i ++ )
   {
      edit = new PMPaletteValueEdit( m_pFiltersWidget );
      m_filterEntries.append( edit );
      connect( edit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
      gl->addWidget( edit, i+1, 0 );
      edit->setIndex( filters[ i ].index( ) );
      edit->setValue( filters[ i ].value( ) );
      edit->show( );
      edit->setReadOnly( readOnly );

      button = new QPushButton( m_pFiltersWidget );
      button->setPixmap( addButtonPixmap );
      m_filterAddButtons.append( button );
      connect( button, SIGNAL( clicked( ) ), SLOT( slotAddFilterEntry( ) ) );
      gl->addWidget( button, i+1, 1 );
      button->show( );
      button->setEnabled( !readOnly );
      QToolTip::add( button, i18n( "Add new filter" ) );

      button = new QPushButton( m_pFiltersWidget );
      button->setPixmap( removeButtonPixmap );
      m_filterRemoveButtons.append( button );
      connect( button, SIGNAL( clicked( ) ), SLOT( slotRemoveFilterEntry( ) ) );
      gl->addWidget( button, i+1, 2 );
      button->show( );
      button->setEnabled( !readOnly );
      QToolTip::add( button, i18n( "Remove filter" ) );
   }

   // ...next the transmit entries
   m_transmitEntries.setAutoDelete( true );
   m_transmitAddButtons.setAutoDelete( true );
   m_transmitRemoveButtons.setAutoDelete( true );
   m_transmitEntries.clear( );
   m_transmitAddButtons.clear( );
   m_transmitRemoveButtons.clear( );
   m_transmitEntries.setAutoDelete( false );
   m_transmitAddButtons.setAutoDelete( false );
   m_transmitRemoveButtons.setAutoDelete( false );

   // recreate the entry edits
   if( m_pTransmitsWidget->layout( ) )
      delete m_pTransmitsWidget->layout( );

   gl = new QGridLayout( m_pTransmitsWidget, ntransmits + 1, 3, 0, KDialog::spacingHint( ) );

   button = new QPushButton( m_pTransmitsWidget );
   button->setPixmap( addButtonPixmap );
   m_transmitAddButtons.append( button );
   connect( button, SIGNAL( clicked( ) ), SLOT( slotAddTransmitEntry( ) ) );
   gl->addWidget( button, 0, 1 );
   button->show( );
   button->setEnabled( !readOnly );
   QToolTip::add( button, i18n( "Add new transmit" ) );

   for( i = 0; i < ntransmits; i ++ )
   {
      edit = new PMPaletteValueEdit( m_pTransmitsWidget );
      m_transmitEntries.append( edit );
      connect( edit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
      gl->addWidget( edit, i+1, 0 );
      edit->setIndex( transmits[ i ].index( ) );
      edit->setValue( transmits[ i ].value( ) );
      edit->show( );
      edit->setReadOnly( readOnly );

      button = new QPushButton( m_pTransmitsWidget );
      button->setPixmap( addButtonPixmap );
      m_transmitAddButtons.append( button );
      connect( button, SIGNAL( clicked( ) ), SLOT( slotAddTransmitEntry( ) ) );
      gl->addWidget( button, i+1, 1 );
      button->show( );
      button->setEnabled( !readOnly );
      QToolTip::add( button, i18n( "Add new transmit" ) );

      button = new QPushButton( m_pTransmitsWidget );
      button->setPixmap( removeButtonPixmap );
      m_transmitRemoveButtons.append( button );
      connect( button, SIGNAL( clicked( ) ), SLOT( slotRemoveTransmitEntry( ) ) );
      gl->addWidget( button, i+1, 2 );
      button->show( );
      button->setEnabled( !readOnly );
      QToolTip::add( button, i18n( "Remove transmit" ) );
   }
}

void PMImageMapEdit::saveContents( )
{
   if( m_pDisplayedObject )
   {
      Base::saveContents( );
      switch( m_pImageFileTypeEdit->currentItem( ) )
      {
         case 0:
            m_pDisplayedObject->setBitmapType( PMImageMap::BitmapGif );
            break;
         case 1:
            m_pDisplayedObject->setBitmapType( PMImageMap::BitmapTga );
            break;
         case 2:
            m_pDisplayedObject->setBitmapType( PMImageMap::BitmapIff );
            break;
         case 3:
            m_pDisplayedObject->setBitmapType( PMImageMap::BitmapPpm );
            break;
         case 4:
            m_pDisplayedObject->setBitmapType( PMImageMap::BitmapPgm );
            break;
         case 5:
            m_pDisplayedObject->setBitmapType( PMImageMap::BitmapPng );
            break;
         case 6:
            m_pDisplayedObject->setBitmapType( PMImageMap::BitmapJpeg );
            break;
         case 7:
            m_pDisplayedObject->setBitmapType( PMImageMap::BitmapTiff );
            break;
         case 8:
            m_pDisplayedObject->setBitmapType( PMImageMap::BitmapSys );
            break;
      }

      switch( m_pInterpolateTypeEdit->currentItem( ) )
      {
         case 0:
            m_pDisplayedObject->setInterpolateType( PMImageMap::InterpolateNone );
            break;
         case 1:
            m_pDisplayedObject->setInterpolateType( PMImageMap::InterpolateBilinear );
            break;
         case 2:
            m_pDisplayedObject->setInterpolateType( PMImageMap::InterpolateNormalized );
            break;
      }

      switch( m_pMapTypeEdit->currentItem( ) )
      {
         case 0:
            m_pDisplayedObject->setMapType( PMImageMap::MapPlanar );
            break;
         case 1:
            m_pDisplayedObject->setMapType( PMImageMap::MapSpherical );
            break;
         case 2:
            m_pDisplayedObject->setMapType( PMImageMap::MapCylindrical );
            break;
         case 3:
            m_pDisplayedObject->setMapType( PMImageMap::MapToroidal );
            break;
      }

      m_pDisplayedObject->setFilters( filters( ) );
      m_pDisplayedObject->setTransmits( transmits( ) );

      m_pDisplayedObject->setBitmapFileName( m_pImageFileNameEdit->text( ) );
      m_pDisplayedObject->enableFilterAll( m_pEnableFilterAllEdit->isChecked( ) );
      m_pDisplayedObject->setFilterAll( m_pFilterAllEdit->value( ) );
      m_pDisplayedObject->enableTransmitAll( m_pEnableTransmitAllEdit->isChecked( ) );
      m_pDisplayedObject->setTransmitAll( m_pTransmitAllEdit->value( ) );
      m_pDisplayedObject->enableOnce( m_pOnceEdit->isChecked( ) );
   }
}

bool PMImageMapEdit::isDataValid( )
{
   if( !m_pFilterAllEdit->isDataValid( ) ) return false;
   if( !m_pTransmitAllEdit->isDataValid( ) ) return false;

   QPtrListIterator<PMPaletteValueEdit> it_filters( m_filterEntries );
   for( ; it_filters.current( ); ++it_filters )
      if( !( it_filters.current( )->isDataValid( ) ) )
         return false;

   QPtrListIterator<PMPaletteValueEdit> it_transmits( m_transmitEntries );
   for( ; it_transmits.current( ); ++it_transmits )
      if( !( it_transmits.current( )->isDataValid( ) ) )
         return false;

   return Base::isDataValid( );
}

void PMImageMapEdit::slotInterpolateTypeChanged( const int /*a*/ )
{
   emit dataChanged( );
}

void PMImageMapEdit::slotImageFileTypeChanged( const int /*a*/ )
{
   emit dataChanged( );
}

void PMImageMapEdit::slotMapTypeChanged( const int /*a*/ )
{
   emit dataChanged( );
}

void PMImageMapEdit::slotImageFileNameChanged( const QString& /*a*/ )
{
   emit dataChanged( );
}

void PMImageMapEdit::slotImageFileBrowseClicked( )
{
   QString str = KFileDialog::getOpenFileName( QString::null, QString::null );

   if( !str.isEmpty() )
   {
      m_pImageFileNameEdit->setText( str );
      emit dataChanged( );
   }
}

void PMImageMapEdit::slotFilterAllClicked( )
{
   if( m_pEnableFilterAllEdit->isChecked( ) )
      m_pFilterAllEdit->setEnabled( true );
   else
      m_pFilterAllEdit->setEnabled( false );
   emit sizeChanged( );
   emit dataChanged( );
}

void PMImageMapEdit::slotTransmitAllClicked( )
{
   if( m_pEnableTransmitAllEdit->isChecked( ) )
      m_pTransmitAllEdit->setEnabled( true );
   else
      m_pTransmitAllEdit->setEnabled( false );
   emit sizeChanged( );
   emit dataChanged( );
}

QValueList<PMPaletteValue> PMImageMapEdit::filters( )
{
   QValueList<PMPaletteValue> entries;
   QPtrListIterator<PMPaletteValueEdit> it( m_filterEntries );

   for( ; it.current( ); ++it )
      entries.append( PMPaletteValue( it.current( )->index( ), it.current( )->value( ) ) );

   return entries;
}

QValueList<PMPaletteValue> PMImageMapEdit::transmits( )
{
   QValueList<PMPaletteValue> entries;
   QPtrListIterator<PMPaletteValueEdit> it( m_transmitEntries );

   for( ; it.current( ); ++it )
      entries.append( PMPaletteValue( it.current( )->index( ), it.current( )->value( ) ) );

   return entries;
}

void PMImageMapEdit::slotAddFilterEntry( )
{
   QValueList<PMPaletteValue> entriesFilters;
   QValueListIterator<PMPaletteValue> it;
   PMPaletteValue newEntry;
   QPushButton* button = ( QPushButton* ) sender( );

   if( button )
   {
      int index = m_filterAddButtons.findRef( button );
      if( index >= 0 )
      {
         entriesFilters = filters( );
         if( index == 0 )
            entriesFilters.prepend( newEntry );
         else
         {
            it = entriesFilters.at( index );
            entriesFilters.insert( it, newEntry );
         }
         displayPaletteEntries( entriesFilters, transmits( ) );
         emit sizeChanged( );
         emit dataChanged( );
      }
   }
}

void PMImageMapEdit::slotRemoveFilterEntry( )
{
   QValueList<PMPaletteValue> entriesFilters;
   QValueListIterator<PMPaletteValue> it;
   QPushButton* button = ( QPushButton* ) sender( );

   if( button )
   {
      int index = m_filterRemoveButtons.findRef( button );
      if( index >= 0 )
      {
         entriesFilters = filters( );
         it = entriesFilters.at( index );
         entriesFilters.remove( it );
         displayPaletteEntries( entriesFilters, transmits( ) );
         emit sizeChanged( );
         emit dataChanged( );
      }
   }
}

void PMImageMapEdit::slotAddTransmitEntry( )
{
   QValueList<PMPaletteValue> entriesTransmits;
   QValueListIterator<PMPaletteValue> it;
   PMPaletteValue newEntry;
   QPushButton* button = ( QPushButton* ) sender( );

   if( button )
   {
      int index = m_transmitAddButtons.findRef( button );
      if( index >= 0 )
      {
         entriesTransmits = transmits( );
         if( index == 0 )
            entriesTransmits.prepend( newEntry );
         else
         {
            it = entriesTransmits.at( index );
            entriesTransmits.insert( it, newEntry );
         }
         displayPaletteEntries( filters( ), entriesTransmits );
         emit sizeChanged( );
         emit dataChanged( );
      }
   }
}

void PMImageMapEdit::slotRemoveTransmitEntry( )
{
   QValueList<PMPaletteValue> entriesTransmits;
   QValueListIterator<PMPaletteValue> it;
   QPushButton* button = ( QPushButton* ) sender( );

   if( button )
   {
      int index = m_transmitRemoveButtons.findRef( button );
      if( index >= 0 )
      {
         entriesTransmits = transmits( );
         it = entriesTransmits.at( index );
         entriesTransmits.remove( it );
         displayPaletteEntries( filters( ), entriesTransmits );
         emit sizeChanged( );
         emit dataChanged( );
      }
   }
}

#include "pmimagemapedit.moc"
