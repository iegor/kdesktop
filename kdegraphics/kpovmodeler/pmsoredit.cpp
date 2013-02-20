/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2002 by Andreas Zehender
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


#include "pmsoredit.h"
#include "pmsor.h"
#include "pmvectorlistedit.h"
#include "pmpart.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <klocale.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <kmessagebox.h>

PMSurfaceOfRevolutionEdit::PMSurfaceOfRevolutionEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}

void PMSurfaceOfRevolutionEdit::createBottomWidgets( )
{
   topLayout( )->addWidget( new QLabel( i18n( "Spline points:" ), this ) );
   
   m_pPoints = new PMVectorListEdit( "u", "v", this );
   connect( m_pPoints, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pPoints, SIGNAL( selectionChanged( ) ),
            SLOT( slotSelectionChanged( ) ) );
   QHBoxLayout* hl = new QHBoxLayout( topLayout( ) );
   hl->addWidget( m_pPoints, 2 );

   m_pAddAbove = new QPushButton( this );
   m_pAddAbove->setPixmap( SmallIcon( "pmaddpointabove" ) );
   m_pAddBelow = new QPushButton( this );
   m_pAddBelow->setPixmap( SmallIcon( "pmaddpoint" ) );
   m_pRemove = new QPushButton( this );
   m_pRemove->setPixmap( SmallIcon( "pmremovepoint" ) );
   connect( m_pAddAbove, SIGNAL( clicked( ) ), SLOT( slotAddPointAbove( ) ) );
   connect( m_pAddBelow, SIGNAL( clicked( ) ), SLOT( slotAddPointBelow( ) ) );
   connect( m_pRemove, SIGNAL( clicked( ) ), SLOT( slotRemovePoint( ) ) );

   QVBoxLayout* bl = new QVBoxLayout( hl );
   bl->addWidget( m_pAddAbove );
   bl->addWidget( m_pAddBelow );
   bl->addWidget( m_pRemove );
   bl->addStretch( 1 );
   
   m_pOpen = new QCheckBox( i18n( "type of the object", "Open" ), this );
   topLayout( )->addWidget( m_pOpen );
   connect( m_pOpen, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );
   m_pSturm = new QCheckBox( i18n( "Sturm" ), this );
   topLayout( )->addWidget( m_pSturm );
   connect( m_pSturm, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );

   Base::createBottomWidgets( );
}

void PMSurfaceOfRevolutionEdit::displayObject( PMObject* o )
{
   if( o->isA( "SurfaceOfRevolution" ) )
   {
      bool readOnly = o->isReadOnly( );
      m_pDisplayedObject = ( PMSurfaceOfRevolution* ) o;

      m_pOpen->setChecked( m_pDisplayedObject->open( ) );
      m_pOpen->setEnabled( !readOnly );
      m_pSturm->setChecked( m_pDisplayedObject->sturm( ) );
      m_pSturm->setEnabled( !readOnly );
      m_pPoints->setVectors( m_pDisplayedObject->points( ), true );
      updateControlPointSelection( );
      updatePointButtons( );

      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMSurfaceOfRevolutionEdit: Can't display object\n";
}

void PMSurfaceOfRevolutionEdit::updateControlPointSelection( )
{
   PMControlPointList cp = part( )->activeControlPoints( );
   PMControlPointListIterator it( cp );
   int i;
   int np = cp.count( ) / 2;

   if( np == m_pPoints->size( ) )
   {
      m_pPoints->blockSelectionUpdates( true );
      bool sb = m_pPoints->signalsBlocked( );
      m_pPoints->blockSignals( true );
      
      m_pPoints->clearSelection( );
      for( i = 0; i < np; i++, ++it )
         if( ( *it )->selected( ) )
            m_pPoints->select( i );
      for( i = 0; i < np; i++, ++it )
         if( ( *it )->selected( ) )
            m_pPoints->select( i );
      
      m_pPoints->blockSignals( sb );
      m_pPoints->blockSelectionUpdates( false );
   }
}

void PMSurfaceOfRevolutionEdit::saveContents( )
{
   if( m_pDisplayedObject )
   {
      m_pDisplayedObject->setPoints( m_pPoints->vectors( ) );
      m_pDisplayedObject->setOpen( m_pOpen->isChecked( ) );
      m_pDisplayedObject->setSturm( m_pSturm->isChecked( ) );
      Base::saveContents( );
   }
}

bool PMSurfaceOfRevolutionEdit::isDataValid( )
{
   if( !m_pPoints->isDataValid( ) )
      return false;

   int np = m_pPoints->size( );
   if( np < 4 )
   {
      KMessageBox::error( this, i18n( "The surface of revolution object needs at least 4 points." ),
                          i18n( "Error" ) );
      return false;
   }
   
   QValueList<PMVector> points = m_pPoints->vectors( );
   QValueListIterator<PMVector> it1 = points.begin( );
   QValueListIterator<PMVector> it2 = it1; ++it2;
   QValueListIterator<PMVector> it3 = it2; ++it3;
   int pnr;

   for( pnr = 0; it3 != points.end( ); ++it1, ++it2, ++it3, pnr++ )
   {
      if( ( pnr == 0 ) || ( pnr == ( np - 3 ) ) )
      {
         if( approxZero( ( *it1 )[1] - ( *it3 )[1], c_sorTolerance ) )
         {
            m_pPoints->setCurrentCell( pnr, 1 );            
            KMessageBox::error( this, i18n( "The v coordinate of point %1 and %2 must be different." )
                                .arg( pnr + 1 ).arg( pnr + 3 ),
                                i18n( "Error" ) );
            return false;
         }
      }

      if( pnr != 0 )
      {
         if( ( ( *it2 )[1] - ( *it1 )[1] ) < c_sorTolerance )
         {
            m_pPoints->setCurrentCell( pnr + 1, 1 );            
            KMessageBox::error( this, i18n( "The v coordinates must be strictly increasing." ),
                                i18n( "Error" ) );
            return false;
         }
      }
   }
   
   return Base::isDataValid( );
}

void PMSurfaceOfRevolutionEdit::slotAddPointAbove( )
{
   int index = m_pPoints->currentRow( );
   if( index >= 0 )
   {
      QValueList<PMVector> points = m_pPoints->vectors( );
      QValueListIterator<PMVector> it = points.at( index );

      if( it != points.end( ) )
      {
         QValueListIterator<PMVector> it2 = it;
         it2--;
         PMVector v;
         if( it2 == points.end( ) )
            v = *it;
         else
            v = ( *it + *it2 ) / 2;
         
         points.insert( it, v );
         m_pPoints->setVectors( points, true );
         updatePointButtons( );
         emit dataChanged( );
      }
   }
}

void PMSurfaceOfRevolutionEdit::slotAddPointBelow( )
{
   int index = m_pPoints->currentRow( );
   if( index >= 0 )
   {
      QValueList<PMVector> points = m_pPoints->vectors( );
      QValueListIterator<PMVector> it = points.at( index );

      if( it != points.end( ) )
      {
         QValueListIterator<PMVector> it2 = it;
         it2++;
         PMVector v;
         if( it2 == points.end( ) )
            v = *it;
         else
            v = ( *it + *it2 ) / 2;
         
         points.insert( it2, v );
         m_pPoints->setVectors( points, true );
         m_pPoints->setCurrentCell( index + 1, m_pPoints->currentColumn( ) );
         updatePointButtons( );
         emit dataChanged( );
      }
   }
}

void PMSurfaceOfRevolutionEdit::slotRemovePoint( )
{
   int row = m_pPoints->currentRow( );

   if( row >= 0 )
   {
      QValueList<PMVector> points = m_pPoints->vectors( );
      QValueListIterator<PMVector> it = points.at( row );
      
      if( it != points.end( ) && points.size( ) > 1 )
      {
         points.remove( it );
         m_pPoints->setVectors( points, true );
         updatePointButtons( );
         emit dataChanged( );
      }
   }
}

void PMSurfaceOfRevolutionEdit::slotSelectionChanged( )
{
   PMControlPointList cp = part( )->activeControlPoints( );
   PMControlPointListIterator it( cp );
   int np = cp.count( ) / 2;
   int i;
   
   if( np == m_pPoints->size( ) )
   {
      for( i = 0; i < np; i++, ++it )
         ( *it )->setSelected( m_pPoints->isSelected( i ) );
      for( i = 0; i < np; i++, ++it )
         ( *it )->setSelected( m_pPoints->isSelected( i ) );
      emit controlPointSelectionChanged( );
   }
   updatePointButtons( );
}

void PMSurfaceOfRevolutionEdit::updatePointButtons( )
{
   int row = m_pPoints->currentRow( );
   m_pAddAbove->setEnabled( row >= 0 );
   m_pAddBelow->setEnabled( row >= 0 );
   m_pRemove->setEnabled( row >= 0 && m_pPoints->size( ) > 4 );
}

#include "pmsoredit.moc"
