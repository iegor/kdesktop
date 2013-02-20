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


#include "pmisosurfaceedit.h"
#include "pmisosurface.h"
#include "pmvectoredit.h"
#include "pmlineedits.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <klocale.h>

PMIsoSurfaceEdit::PMIsoSurfaceEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}

void PMIsoSurfaceEdit::createTopWidgets( )
{
   int i;
   QGridLayout* gl;
   QHBoxLayout* hl;

   Base::createTopWidgets( );

   m_pFunction = new QLineEdit( this );
   m_pContainedBy = new QComboBox( false, this );
   m_pContainedBy->insertItem( i18n( "Box" ) );
   m_pContainedBy->insertItem( i18n( "Sphere" ) );

   m_pCorner1 = new PMVectorEdit( "x", "y", "z", this );
   m_pCorner2 = new PMVectorEdit( "x", "y", "z", this );
   m_pCenter = new PMVectorEdit( "x", "y", "z", this );
   m_pRadius = new PMFloatEdit( this );
   m_pCorner1Label = new QLabel( i18n( "Corner1:" ), this );
   m_pCorner2Label = new QLabel( i18n( "Corner2:" ), this );
   m_pCenterLabel = new QLabel( i18n( "Center:" ), this );
   m_pRadiusLabel = new QLabel( i18n( "Radius:" ), this );

   m_pThreshold = new PMFloatEdit( this );
   m_pAccuracy = new PMFloatEdit( this );
   m_pAccuracy->setValidation( true, 1e-8, false, 0 );
   m_pMaxGradient = new PMFloatEdit( this );
   m_pMaxGradient->setValidation( true, 1e-8, false, 0 );
   m_pEvaluate = new QCheckBox( i18n( "Adapt maximum gradient" ), this );
   for( i = 0; i < 3; i++ )
      m_pEvaluateValue[i] = new PMFloatEdit( this );
   m_pMaxTrace = new PMIntEdit( this );
   m_pMaxTrace->setValidation( true, 1, false, 0 );
   m_pAllIntersections = new QCheckBox( i18n( "All intersections" ), this );
   m_pOpen = new QCheckBox( i18n( "type of the object", "Open" ), this );

   gl = new QGridLayout( topLayout( ), 8, 2 );
   gl->addWidget( new QLabel( i18n( "Function:" ), this ), 0, 0 );
   gl->addWidget( m_pFunction, 0, 1 );
   gl->addWidget( new QLabel( i18n( "Container:" ), this ), 1, 0 );
   gl->addWidget( m_pContainedBy, 1, 1 );
   gl->addWidget( m_pCorner1Label, 2, 0 );
   gl->addWidget( m_pCorner1, 2, 1 );
   gl->addWidget( m_pCorner2Label, 3, 0 );
   gl->addWidget( m_pCorner2, 3, 1 );
   gl->addWidget( m_pCenterLabel, 4, 0 );
   gl->addWidget( m_pCenter, 4, 1 );
   gl->addWidget( m_pRadiusLabel, 5, 0 );
   gl->addWidget( m_pRadius, 5, 1 );
   gl->addWidget( new QLabel( i18n( "Threshold:" ), this ), 6, 0 );
   gl->addWidget( m_pThreshold, 6, 1 );
   gl->addWidget( new QLabel( i18n( "Accuracy:" ), this ), 7, 0 );
   gl->addWidget( m_pAccuracy, 7, 1 );

   hl = new QHBoxLayout( topLayout( ) );
   hl->addWidget( new QLabel( i18n( "Maximum gradient:" ), this ) );
   hl->addWidget( m_pMaxGradient );

   topLayout( )->addWidget( m_pEvaluate );
   hl = new QHBoxLayout( topLayout( ) );
   hl->addWidget( new QLabel( i18n( "Values:" ), this ) );
   for( i = 0; i < 3; i++ )
   {
      hl->addWidget( new QLabel( QString( "P%1" ).arg( i ), this ) );
      hl->addWidget( m_pEvaluateValue[i] );
   }

   hl = new QHBoxLayout( topLayout( ) );
   hl->addWidget( new QLabel( i18n( "Maximum traces:" ), this ) );
   hl->addWidget( m_pMaxTrace );
   topLayout( )->addWidget( m_pAllIntersections );
   topLayout( )->addWidget( m_pOpen );

   connect( m_pFunction, SIGNAL( textChanged( const QString& ) ),
            SLOT( textChanged( const QString& ) ) );
   connect( m_pContainedBy, SIGNAL( activated( int ) ),
            SLOT( currentChanged( int ) ) );
   connect( m_pCorner1, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pCorner2, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pCenter, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pRadius, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pThreshold, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pAccuracy, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pMaxGradient, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pEvaluate, SIGNAL( toggled( bool ) ),
            SLOT( evaluateToggled( bool ) ) );
   connect( m_pMaxTrace, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   for( i = 0; i < 3; i++ )
      connect( m_pEvaluateValue[i], SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pOpen, SIGNAL( toggled( bool ) ), SLOT( toggled( bool ) ) );
   connect( m_pAllIntersections, SIGNAL( toggled( bool ) ),
            SLOT( allToggled( bool ) ) );
}

void PMIsoSurfaceEdit::displayObject( PMObject* o )
{
   int i;

   if( o->isA( "IsoSurface" ) )
   {
      bool readOnly = o->isReadOnly( );
      m_pDisplayedObject = ( PMIsoSurface* ) o;

      m_pFunction->setText( m_pDisplayedObject->function( ) );
      if( m_pDisplayedObject->containedBy( ) == PMIsoSurface::Box )
      {
         m_pContainedBy->setCurrentItem( 0 );
         m_pCorner1Label->show( );
         m_pCorner2Label->show( );
         m_pCorner1->show( );
         m_pCorner2->show( );
         m_pCenterLabel->hide( );
         m_pCenter->hide( );
         m_pRadiusLabel->hide( );
         m_pRadius->hide( );
      }
      else
      {
         m_pContainedBy->setCurrentItem( 1 );
         m_pCorner1Label->hide( );
         m_pCorner2Label->hide( );
         m_pCorner1->hide( );
         m_pCorner2->hide( );
         m_pCenterLabel->show( );
         m_pCenter->show( );
         m_pRadiusLabel->show( );
         m_pRadius->show( );
      }

      m_pCorner1->setVector( m_pDisplayedObject->corner1( ) );
      m_pCorner2->setVector( m_pDisplayedObject->corner2( ) );
      m_pCenter->setVector( m_pDisplayedObject->center( ) );
      m_pRadius->setValue( m_pDisplayedObject->radius( ) );
      m_pThreshold->setValue( m_pDisplayedObject->threshold( ) );
      m_pAccuracy->setValue( m_pDisplayedObject->accuracy( ) );
      m_pMaxGradient->setValue( m_pDisplayedObject->maxGradient( ) );
      bool ev = m_pDisplayedObject->evaluate( );
      m_pEvaluate->setChecked( ev );

      for( i = 0; i < 3; i++ )
      {
         m_pEvaluateValue[i]->setValue( m_pDisplayedObject->evaluateValue( i ) );
         m_pEvaluateValue[i]->setEnabled( ev );
      }

      m_pOpen->setChecked( m_pDisplayedObject->open( ) );
      m_pMaxTrace->setValue( m_pDisplayedObject->maxTrace( ) );
      bool all = m_pDisplayedObject->allIntersections( );
      m_pAllIntersections->setChecked( all );
      m_pMaxTrace->setEnabled( !all );

      m_pFunction->setReadOnly( readOnly );
      m_pContainedBy->setEnabled( !readOnly );
      m_pCorner1->setReadOnly( readOnly );
      m_pCorner2->setReadOnly( readOnly );
      m_pCenter->setReadOnly( readOnly );
      m_pRadius->setReadOnly( readOnly );
      m_pThreshold->setReadOnly( readOnly );
      m_pAccuracy->setReadOnly( readOnly );
      m_pMaxGradient->setReadOnly( readOnly );
      m_pEvaluate->setEnabled( !readOnly );
      for( i = 0; i < 3; i++ )
         m_pEvaluateValue[i]->setReadOnly( readOnly );
      m_pOpen->setEnabled( !readOnly );
      m_pMaxTrace->setReadOnly( readOnly );
      m_pAllIntersections->setEnabled( !readOnly );

      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMIsoSurfaceEdit: Can't display object\n";
}

void PMIsoSurfaceEdit::saveContents( )
{
   int i;
   if( m_pDisplayedObject )
   {
      Base::saveContents( );
      m_pDisplayedObject->setFunction( m_pFunction->text( ) );
      if( m_pContainedBy->currentItem( ) == 0 )
      {
         m_pDisplayedObject->setContainedBy( PMIsoSurface::Box );
         m_pDisplayedObject->setCorner1( m_pCorner1->vector( ) );
         m_pDisplayedObject->setCorner2( m_pCorner2->vector( ) );
      }
      else
      {
         m_pDisplayedObject->setContainedBy( PMIsoSurface::Sphere );
         m_pDisplayedObject->setCenter( m_pCenter->vector( ) );
         m_pDisplayedObject->setRadius( m_pRadius->value( ) );
      }
      m_pDisplayedObject->setThreshold( m_pThreshold->value( ) );
      m_pDisplayedObject->setAccuracy( m_pAccuracy->value( ) );
      m_pDisplayedObject->setMaxGradient( m_pMaxGradient->value( ) );
      m_pDisplayedObject->setEvaluate( m_pEvaluate->isChecked( ) );
      if( m_pEvaluate->isChecked( ) )
         for( i = 0; i < 3; i++ )
            m_pDisplayedObject->setEvaluateValue( i, m_pEvaluateValue[i]->value( ) );
      m_pDisplayedObject->setOpen( m_pOpen->isChecked( ) );
      m_pDisplayedObject->setAllIntersections( m_pAllIntersections->isChecked( ) );
      if( !m_pAllIntersections->isChecked( ) )
         m_pDisplayedObject->setMaxTrace( m_pMaxTrace->value( ) );
   }
}

bool PMIsoSurfaceEdit::isDataValid( )
{
   int i;

   if( m_pContainedBy->currentItem( ) == 0 )
   {
      if( !m_pCorner1->isDataValid( ) )
         return false;
      if( !m_pCorner2->isDataValid( ) )
         return false;
   }
   else
   {
      if( !m_pCenter->isDataValid( ) )
         return false;
      if( !m_pRadius->isDataValid( ) )
         return false;
   }
   if( !m_pThreshold->isDataValid( ) )
      return false;
   if( !m_pAccuracy->isDataValid( ) )
      return false;
   if( !m_pMaxGradient->isDataValid( ) )
      return false;
   if( m_pEvaluate->isChecked( ) )
      for( i = 0; i < 3; i++ )
         if( !m_pEvaluateValue[i]->isDataValid( ) )
            return false;
   if( !m_pAllIntersections->isChecked( ) && !m_pMaxTrace->isDataValid( ) )
      return false;
   return Base::isDataValid( );
}

void PMIsoSurfaceEdit::textChanged( const QString& )
{
   emit dataChanged( );
}

void PMIsoSurfaceEdit::currentChanged( int i )
{
   if( i == 0 )
   {
      m_pCorner1Label->show( );
      m_pCorner2Label->show( );
      m_pCorner1->show( );
      m_pCorner2->show( );
      m_pCenterLabel->hide( );
      m_pCenter->hide( );
      m_pRadiusLabel->hide( );
      m_pRadius->hide( );
   }
   else
   {
      m_pCorner1Label->hide( );
      m_pCorner2Label->hide( );
      m_pCorner1->hide( );
      m_pCorner2->hide( );
      m_pCenterLabel->show( );
      m_pCenter->show( );
      m_pRadiusLabel->show( );
      m_pRadius->show( );
   }
   emit dataChanged( );
}

void PMIsoSurfaceEdit::evaluateToggled( bool yes )
{
   int i;
   for( i = 0; i < 3; i++ )
      m_pEvaluateValue[i]->setEnabled( yes );
   emit dataChanged( );
}

void PMIsoSurfaceEdit::allToggled( bool yes )
{
   m_pMaxTrace->setEnabled( !yes );
   emit dataChanged( );
}

void PMIsoSurfaceEdit::toggled( bool )
{
   emit dataChanged( );
}

#include "pmisosurfaceedit.moc"
