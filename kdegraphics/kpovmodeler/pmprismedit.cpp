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


#include "pmprismedit.h"
#include "pmprism.h"
#include "pmvectoredit.h"
#include "pmlineedits.h"
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

PMPrismEdit::PMPrismEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
   m_lastSplineType = 0;
}

PMPrismEdit::~PMPrismEdit( )
{
}

void PMPrismEdit::createTopWidgets( )
{
   Base::createTopWidgets( );

   QHBoxLayout* hl = new QHBoxLayout( topLayout( ) );
   hl->addWidget( new QLabel( i18n( "Spline type:" ), this ) );
   m_pSplineType = new QComboBox( false, this );
   m_pSplineType->insertItem( i18n( "Linear Spline" ) );
   m_pSplineType->insertItem( i18n( "Quadratic Spline" ) );
   m_pSplineType->insertItem( i18n( "Cubic Spline" ) );
   m_pSplineType->insertItem( i18n( "Bezier Spline" ) );
   hl->addWidget( m_pSplineType );

   hl = new QHBoxLayout( topLayout( ) );
   hl->addWidget( new QLabel( i18n( "Sweep type:" ), this ) );
   m_pSweepType = new QComboBox( false, this );
   m_pSweepType->insertItem( i18n( "Linear Sweep" ) );
   m_pSweepType->insertItem( i18n( "Conic Sweep" ) );
   hl->addWidget( m_pSweepType );

   connect( m_pSplineType, SIGNAL( activated( int ) ),
            SLOT( slotTypeChanged( int ) ) );
   connect( m_pSweepType, SIGNAL( activated( int ) ),
            SLOT( slotSweepChanged( int ) ) );

   hl = new QHBoxLayout( topLayout( ) );
   QGridLayout* gl = new QGridLayout( hl, 2, 2 );
   gl->addWidget( new QLabel( i18n( "Height 1:" ), this ), 0, 0 );
   m_pHeight1 = new PMFloatEdit( this );
   gl->addWidget( m_pHeight1, 0, 1 );
   connect( m_pHeight1, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   
   gl->addWidget( new QLabel( i18n( "Height 2:" ), this ), 1, 0 );
   m_pHeight2 = new PMFloatEdit( this );
   gl->addWidget( m_pHeight2, 1, 1 );
   connect( m_pHeight2, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   hl->addStretch( 1 );
}

void PMPrismEdit::createBottomWidgets( )
{
   m_pEditWidget = new QWidget( this );
   topLayout( )->addWidget( m_pEditWidget );
   m_pOpen = new QCheckBox( i18n( "type of the object", "Open" ), this );
   topLayout( )->addWidget( m_pOpen );
   m_pSturm = new QCheckBox( i18n( "Sturm" ), this );
   topLayout( )->addWidget( m_pSturm );
   connect( m_pSturm, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pOpen, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );

   Base::createBottomWidgets( );
}

void PMPrismEdit::displayObject( PMObject* o )
{
   if( o->isA( "Prism" ) )
   {
      bool readOnly = o->isReadOnly( );
      m_pDisplayedObject = ( PMPrism* ) o;

      switch( m_pDisplayedObject->splineType( ) )
      {
         case PMPrism::LinearSpline:
            m_pSplineType->setCurrentItem( 0 );
            break;
         case PMPrism::QuadraticSpline:
            m_pSplineType->setCurrentItem( 1 );
            break;
         case PMPrism::CubicSpline:
            m_pSplineType->setCurrentItem( 2 );
            break;
         case PMPrism::BezierSpline:
            m_pSplineType->setCurrentItem( 3 );
            break;
      }
      m_pSplineType->setEnabled( !readOnly );
      switch( m_pDisplayedObject->sweepType( ) )
      {
         case PMPrism::LinearSweep:
            m_pSweepType->setCurrentItem( 0 );
            break;
         case PMPrism::ConicSweep:
            m_pSweepType->setCurrentItem( 1 );
            break;
      }
      m_pHeight1->setValue( m_pDisplayedObject->height1( ) );
      m_pHeight1->setReadOnly( readOnly );
      m_pHeight2->setValue( m_pDisplayedObject->height2( ) );
      m_pHeight2->setReadOnly( readOnly );
      m_pSweepType->setEnabled( !readOnly );
      m_pSturm->setChecked( m_pDisplayedObject->sturm( ) );
      m_pSturm->setEnabled( !readOnly );
      m_pOpen->setChecked( m_pDisplayedObject->open( ) );
      m_pOpen->setEnabled( !readOnly );
      displayPoints( m_pDisplayedObject->points( ) );

      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMPrismEdit: Can't display object\n";
}

void PMPrismEdit::displayPoints( const QValueList< QValueList<PMVector> >& sp )
{
   bool readOnly = m_pDisplayedObject->isReadOnly( );

   // (re)create the edit widget if necessary
   createEdits( sp );
   
   QValueList< QValueList<PMVector> >::ConstIterator spit = sp.begin( );
   QPtrListIterator< PMVectorListEdit > seit( m_points );
   QPtrListIterator< QPushButton > sbit1( m_removeButtons );

   // display the points
   for( ; ( spit != sp.end( ) ) && *seit; ++spit, ++seit, ++sbit1 )
   {
      ( *seit )->setVectors( *spit );
      ( *seit )->setReadOnly( readOnly );
      ( *sbit1 )->setEnabled( !readOnly && ( *spit ).size( ) > 3 );
   }

   QPtrListIterator< QPushButton > sbit2( m_addAboveButtons );
   QPtrListIterator< QPushButton > sbit3( m_addBelowButtons );
   for( ; *sbit2; ++sbit2 )
      ( *sbit2 )->setEnabled( !readOnly );
   for( ; *sbit3; ++sbit3 )
      ( *sbit3 )->setEnabled( !readOnly );

   QPtrListIterator<QPushButton> bit1( m_subPrismAddButtons );
   for( ; *bit1; ++bit1 )
      ( *bit1 )->setEnabled( !readOnly );
   QPtrListIterator<QPushButton> bit2( m_subPrismRemoveButtons );
   for( ; *bit2; ++bit2 )
      ( *bit2 )->setEnabled( !readOnly && sp.size( ) > 1 );
   updateControlPointSelection( );
}

void PMPrismEdit::createEdits( const QValueList< QValueList<PMVector> >& sp )
{
   int st = m_pSplineType->currentItem( );
   
   if( sp.size( ) != m_points.count( ) )
   {
      deleteEdits( );

      QPixmap addPixmap = SmallIcon( "pmaddpoint" );
      QPixmap removePixmap = SmallIcon( "pmremovepoint" );
      QPixmap addPrismPixmap = SmallIcon( "pmaddsubprism" );
      QVBoxLayout* tvl = new QVBoxLayout( m_pEditWidget,
                                          0, KDialog::spacingHint( ) );
      QHBoxLayout* hl = 0;
      QVBoxLayout* vl;
      QLabel* label = 0;
      QPushButton* button = 0;
      PMVectorListEdit* vle;
      int spnr = 0;
      
      for( spnr = 0; spnr < ( signed ) sp.size( ); spnr++ )
      {
         // create all edits for one sub prism
         hl = new QHBoxLayout( tvl );
         label = new QLabel( i18n( "Sub prism %1:" ).arg( spnr + 1 ),
                             m_pEditWidget );
         hl->addWidget( label );
         hl->addStretch( 1 );
         m_labels.append( label );
         label->show( );
         
         button = new QPushButton( m_pEditWidget );
         button->setPixmap( addPrismPixmap );
         m_subPrismAddButtons.append( button );
         connect( button, SIGNAL( clicked( ) ), SLOT( slotAddSubPrism( ) ) );
         hl->addWidget( button );
         button->show( );
         QToolTip::add( button, i18n( "Add sub prism" ) );
         
         button = new QPushButton( m_pEditWidget );
         button->setPixmap( removePixmap );
         m_subPrismRemoveButtons.append( button );
         connect( button, SIGNAL( clicked( ) ), SLOT( slotRemoveSubPrism( ) ) );
         hl->addWidget( button );
         button->show( );
         if( sp.size( ) < 2 )
            button->setEnabled( false );
         QToolTip::add( button, i18n( "Remove sub prism" ) );

         hl = new QHBoxLayout( tvl );
         
         vle = new PMVectorListEdit( "x", "z", m_pEditWidget );
         m_points.append( vle );
         connect( vle, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
         connect( vle, SIGNAL( selectionChanged( ) ),
                  SLOT( slotSelectionChanged( ) ) );
         hl->addWidget( vle, 2 );
         vle->show( );

         vl = new QVBoxLayout( hl );
         
         button = new QPushButton( m_pEditWidget );
         button->setPixmap( SmallIcon( "pmaddpointabove" ) );
         connect( button, SIGNAL( clicked( ) ), SLOT( slotAddPointAbove( ) ) );
         m_addAboveButtons.append( button );
         button->show( );
         vl->addWidget( button );
         button = new QPushButton( m_pEditWidget );
         button->setPixmap( SmallIcon( "pmaddpoint" ) );
         connect( button, SIGNAL( clicked( ) ), SLOT( slotAddPointBelow( ) ) );
         m_addBelowButtons.append( button );
         button->show( );
         vl->addWidget( button );
         button = new QPushButton( m_pEditWidget );
         button->setPixmap( SmallIcon( "pmremovepoint" ) );
         connect( button, SIGNAL( clicked( ) ), SLOT( slotRemovePoint( ) ) );
         m_removeButtons.append( button );
         button->show( );
         vl->addWidget( button );

         vl->addStretch( 1 );
         
         tvl->addSpacing( KDialog::spacingHint( ) );
      }
      
      hl = new QHBoxLayout( tvl );
      label = new QLabel( i18n( "New sub prism" ), m_pEditWidget );
      hl->addWidget( label );
      hl->addStretch( 1 );
      m_labels.append( label );
      label->show( );
         
      button = new QPushButton( m_pEditWidget );
      button->setPixmap( addPrismPixmap );
      m_subPrismAddButtons.append( button );
      connect( button, SIGNAL( clicked( ) ), SLOT( slotAddSubPrism( ) ) );
      hl->addWidget( button );
      button->show( );
      QToolTip::add( button, i18n( "Append sub prism" ) );
   }

   QPtrListIterator< PMVectorListEdit > vlit( m_points );
   QValueList< QValueList< PMVector > >::ConstIterator spit;
   PMVectorListEdit* vle = 0;
   bool newSize = false;
   
   for( spit = sp.begin( ); spit != sp.end( ); ++spit, ++vlit )
   {
      int lines = ( *spit ).count( );
      
      vle = *vlit;
      if( ( vle->size( ) != lines ) /*|| ( st != m_lastSplineType )*/ )
      {
         newSize = true;
         vle->setSize( lines );
      }
   }
   if( newSize )
   {
      m_pEditWidget->updateGeometry( );
      emit sizeChanged( );
   }

   m_lastSplineType = st;
}

void PMPrismEdit::deleteEdits( )
{
   m_labels.setAutoDelete( true );
   m_labels.clear( );
   m_labels.setAutoDelete( false );
   m_subPrismAddButtons.setAutoDelete( true );
   m_subPrismAddButtons.clear( );
   m_subPrismAddButtons.setAutoDelete( false );
   m_subPrismRemoveButtons.setAutoDelete( true );
   m_subPrismRemoveButtons.clear( );
   m_subPrismRemoveButtons.setAutoDelete( false );
   m_addAboveButtons.setAutoDelete( true );
   m_addAboveButtons.clear( );
   m_addAboveButtons.setAutoDelete( false );
   m_addBelowButtons.setAutoDelete( true );
   m_addBelowButtons.clear( );
   m_addBelowButtons.setAutoDelete( false );
   m_removeButtons.setAutoDelete( true );
   m_removeButtons.clear( );
   m_removeButtons.setAutoDelete( false );
   m_points.setAutoDelete( true );
   m_points.clear( );
   m_points.setAutoDelete( false );
   
   if( m_pEditWidget->layout( ) )
      delete m_pEditWidget->layout( );
}

QValueList< QValueList<PMVector> > PMPrismEdit::splinePoints( )
{
   QPtrListIterator< PMVectorListEdit > it( m_points );
   QValueList< QValueList<PMVector> > values;

   for( ; it.current( ); ++it )
      values.append( ( *it )->vectors( ) );
   
   return values;
}

void PMPrismEdit::saveContents( )
{
   if( m_pDisplayedObject )
   {
      m_pDisplayedObject->setPoints( splinePoints( ) );

      switch( m_pSplineType->currentItem( ) )
      {
         case 0:
            m_pDisplayedObject->setSplineType( PMPrism::LinearSpline );
            break;
         case 1:
            m_pDisplayedObject->setSplineType( PMPrism::QuadraticSpline );
            break;
         case 2:
            m_pDisplayedObject->setSplineType( PMPrism::CubicSpline );
            break;
         case 3:
            m_pDisplayedObject->setSplineType( PMPrism::BezierSpline );
            break;
      }
      switch( m_pSweepType->currentItem( ) )
      {
         case 0:
            m_pDisplayedObject->setSweepType( PMPrism::LinearSweep );
            break;
         case 1:
            m_pDisplayedObject->setSweepType( PMPrism::ConicSweep );
            break;
      }
      m_pDisplayedObject->setSturm( m_pSturm->isChecked( ) );
      m_pDisplayedObject->setOpen( m_pOpen->isChecked( ) );
      m_pDisplayedObject->setHeight1( m_pHeight1->value( ) );
      m_pDisplayedObject->setHeight2( m_pHeight2->value( ) );
      Base::saveContents( );
   }
}

bool PMPrismEdit::isDataValid( )
{
   QPtrListIterator< PMVectorListEdit > it( m_points );
   for( ; it.current( ); ++it )
      if( !it.current( )->isDataValid( ) )
         return false;
   
   for( it.toFirst( ); it.current( ); ++it )
   {
      int np = it.current( )->size( );
      switch( m_pSplineType->currentItem( ) )
      {
         case 0:
            if( np < 3 )
            {
               KMessageBox::error( this, i18n( "Linear splines need at least 3 points." ),
                                   i18n( "Error" ) );
               return false;
            }
            break;
         case 1:
            if( np < 4 )
            {
               KMessageBox::error( this, i18n( "Quadratic splines need at least 4 points." ),
                                   i18n( "Error" ) );
               return false;
            }
            break;
         case 2:
            if( np < 5 )
            {
               KMessageBox::error( this, i18n( "Cubic splines need at least 5 points." ),
                                   i18n( "Error" ) );
               return false;
            }
            break;
         case 3:
            if( ( np < 3 ) || ( ( np % 3 ) != 0 ) )
            {
               KMessageBox::error( this, i18n( "Bezier splines need 3 points for each segment." ),
                                   i18n( "Error" ) );
               return false;
            }
            break;
      }
   }
   
   return Base::isDataValid( );
}

void PMPrismEdit::slotTypeChanged( int )
{
   displayPoints( splinePoints( ) );
   emit dataChanged( );
   emit sizeChanged( );
}

void PMPrismEdit::slotSweepChanged( int )
{
   emit dataChanged( );
}

void PMPrismEdit::slotAddPointAbove( )
{
   QPushButton* bt = ( QPushButton* ) sender( );
   if( bt )
   {
      int subIndex = m_addAboveButtons.findRef( bt );
      if( subIndex >= 0 )
      {
         PMVectorListEdit* ed = m_points.at( subIndex );
         int index = ed->currentRow( );
         if( index >= 0 && index < ed->size( ) )
         {
            QValueList<PMVector> points = ed->vectors( );
            QValueListIterator<PMVector> it = points.at( index );
            
            PMVector newPoint = *it;
            if( index != 0 )
            {
               --it;
               newPoint = ( newPoint + *it ) / 2;
               ++it;
            }
            points.insert( it, newPoint );

            ed->setSize( points.size( ) );
            ed->setVectors( points );
            if( points.size( ) > 3 )
               m_removeButtons.at( subIndex )->setEnabled( true );

            emit dataChanged( );
            emit sizeChanged( );
         }
      }
   }
}

void PMPrismEdit::slotAddPointBelow( )
{
   QPushButton* bt = ( QPushButton* ) sender( );
   if( bt )
   {
      int subIndex = m_addBelowButtons.findRef( bt );
      if( subIndex >= 0 )
      {
         PMVectorListEdit* ed = m_points.at( subIndex );
         int index = ed->currentRow( );
         if( index >= 0 && index < ed->size( ) )
         {
            QValueList<PMVector> points = ed->vectors( );
            QValueListIterator<PMVector> it = points.at( index );
            
            PMVector newPoint = *it;
            ++it;
            
            if( it != points.end( ) )
               newPoint = ( newPoint + *it ) / 2;
            
            points.insert( it, newPoint );

            ed->setSize( points.size( ) );
            ed->setVectors( points );
            ed->setCurrentCell( index + 1, ed->currentColumn( ) );
            if( points.size( ) > 3 )
               m_removeButtons.at( subIndex )->setEnabled( true );

            emit dataChanged( );
            emit sizeChanged( );
         }
      }
   }
}

void PMPrismEdit::slotRemovePoint( )
{
   QPushButton* bt = ( QPushButton* ) sender( );
   if( bt )
   {
      int subIndex = m_removeButtons.findRef( bt );
      if( subIndex >= 0 )
      {
         PMVectorListEdit* ed = m_points.at( subIndex );
         int index = ed->currentRow( );
         if( index >= 0 && index < ed->size( ) )
         {
            QValueList<PMVector> points = ed->vectors( );
            QValueListIterator<PMVector> it = points.at( index );
            
            points.remove( it );

            ed->setSize( points.size( ) );
            ed->setVectors( points );
            if( points.size( ) <= 3 )
               m_removeButtons.at( subIndex )->setEnabled( false );

            emit dataChanged( );
            emit sizeChanged( );
         }
      }
   }
}

void PMPrismEdit::slotAddSubPrism( )
{
   if( m_pSplineType->currentItem( ) == 3 )
   {
      KMessageBox::information( this, i18n( "Sub prisms do not work with "
                                            "bezier splines in POV-Ray 3.1." ),
                                i18n( "Warning" ), "subPrismWithBezierSplines" );
   }
   
   QPushButton* button = ( QPushButton* ) sender( );
   if( button )
   {
      int index = m_subPrismAddButtons.findRef( button );
      if( index >= 0 )
      {
         QValueList< QValueList<PMVector> > points = splinePoints( );
         QValueList< QValueList<PMVector> >::Iterator it = points.at( index );
         QValueList<PMVector> newSubPrism;

         if( it != points.begin( ) )
         {
            --it;
            newSubPrism = *it;
            ++it;
            
            // find middle point
            PMVector mid( 2 );
            int num = 0;
            QValueList<PMVector>::Iterator pit = newSubPrism.begin( );
            for( ; pit != newSubPrism.end( ); ++pit, ++num )
               mid += *pit;
            if( num > 0 )
               mid /= num;
            for( pit = newSubPrism.begin( ); pit != newSubPrism.end( ); ++pit )
               *pit = ( *pit - mid ) * 0.8 + mid;
         }
         else
            newSubPrism = *it;

         points.insert( it, newSubPrism );
         displayPoints( points );
         emit dataChanged( );
         emit sizeChanged( );
      }
   }
}

void PMPrismEdit::slotRemoveSubPrism( )
{
   QPushButton* button = ( QPushButton* ) sender( );
   if( button )
   {
      int index = m_subPrismRemoveButtons.findRef( button );
      if( index >= 0 )
      {
         QValueList< QValueList<PMVector> > points = splinePoints( );
         QValueList< QValueList<PMVector> >::Iterator it = points.at( index );

         if( points.count( ) > 1 )
         {
            points.remove( it );
            displayPoints( points );
            emit dataChanged( );
            emit sizeChanged( );
         }
      }
   }
}

void PMPrismEdit::slotSelectionChanged( )
{
   PMVectorListEdit* edit = ( PMVectorListEdit* ) sender( );
   if( edit )
   {
      QValueList< QValueList< PMVector > > points = m_pDisplayedObject->points( );

      if( m_points.count( ) == points.size( ) )
      {
         int i;
         bool changed = false;
         QValueList< QValueList< PMVector > >::Iterator spit;
         PMControlPointList cp = part( )->activeControlPoints( );
         PMControlPointListIterator it( cp );  ++it; ++it;
         QPtrListIterator<PMVectorListEdit> edit( m_points );
         
         for( spit = points.begin( ); spit != points.end( ) && it.current( );
              ++spit, ++edit )
         {
            int np = ( *spit ).size( );
            
            if( ( *edit )->size( ) == np )
            {
               for( i = 0; i < np && it.current( ); i++, ++it )
                  ( *it )->setSelected( ( *edit )->isSelected( i ) );
               changed = true;
            }
            else
               for( i = 0; i < np; i++ )
                  ++it;
         }
         if( changed )
            emit controlPointSelectionChanged( );
      }
   }
}

void PMPrismEdit::updateControlPointSelection( )
{
   QValueList< QValueList< PMVector > > points = m_pDisplayedObject->points( );
   
   if( m_points.count( ) == points.size( ) )
   {
      QValueList< QValueList< PMVector > >::Iterator spit;
      PMControlPointList cp = part( )->activeControlPoints( );
      PMControlPointListIterator it( cp );  ++it; ++it;
      QPtrListIterator<PMVectorListEdit> edit( m_points );
      
      for( spit = points.begin( ); spit != points.end( ) && it.current( );
           ++spit, ++edit )
      {
         PMVectorListEdit* vl = *edit;
         int np = ( *spit ).size( );
         int i;
         
         if( vl->size( ) == np )
         {
            vl->blockSelectionUpdates( true );
            bool sb = vl->signalsBlocked( );
            vl->blockSignals( true );

            vl->clearSelection( );
            for( i = 0; i < np && it.current( ); i++, ++it )
               if( ( *it )->selected( ) )
                  vl->select( i );
            
            vl->blockSignals( sb );
            vl->blockSelectionUpdates( false );
         }
         else
            for( i = 0; i < np; i++ )
               ++it;
      }
   }
}

#include "pmprismedit.moc"
