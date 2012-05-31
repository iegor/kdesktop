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


#include "pmpolynomedit.h"
#include "pmpolynom.h"
#include "pmpolynomexponents.h"
#include "pmlineedits.h"
#include "pmformulalabel.h"

#include <qlayout.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <klocale.h>
#include <kdialog.h>

PMPolynomEdit::PMPolynomEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
   m_currentOrder = 0;
   m_readOnly = false;
}

void PMPolynomEdit::createTopWidgets( )
{
   Base::createTopWidgets( );

   QHBoxLayout* hl = new QHBoxLayout( topLayout( ) );
   hl->addWidget( new QLabel( i18n( "Order" ), this ) );
   m_pOrder = new QSpinBox( 2, 7, 1, this );
   hl->addWidget( m_pOrder );
   hl->addStretch( 1 );
   connect( m_pOrder, SIGNAL( valueChanged( int ) ), SLOT( slotOrderChanged( int ) ) );
   
   topLayout( )->addWidget( new QLabel( i18n( "Formula:" ), this ) );
   m_pPolyWidget = new QWidget( this );
   topLayout( )->addWidget( m_pPolyWidget );
   m_pSturm = new QCheckBox( i18n( "Sturm" ), this );
   topLayout( )->addWidget( m_pSturm );
   connect( m_pSturm, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );
}

void PMPolynomEdit::displayObject( PMObject* o )
{
   if( o->isA( "Polynom" ) )
   {
      bool readOnly = o->isReadOnly( );
      m_readOnly = readOnly;
      m_pDisplayedObject = ( PMPolynom* ) o;

      displayCoefficients( m_pDisplayedObject->coefficients( ),
                           m_pDisplayedObject->polynomOrder( ),
                           m_pDisplayedObject->polynomOrder( ) );
      
      m_pSturm->setChecked( m_pDisplayedObject->sturm( ) );
      m_pSturm->setEnabled( !readOnly );
      if( m_pDisplayedObject->polynomOrder( ) == 2 )
         m_pSturm->hide( );
      else
         m_pSturm->show( );

      bool sb = m_pOrder->signalsBlocked( );
      m_pOrder->blockSignals( true );
      m_pOrder->setValue( m_pDisplayedObject->polynomOrder( ) );
      m_pOrder->blockSignals( sb );
      
      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMPolynomEdit: Can't display object\n";
}

void PMPolynomEdit::displayCoefficients( const PMVector& co, int cOrder,
                                         int dOrder )
{
   QValueList<PMPolynomExponents>& polynom
      = PMPolynomExponents::polynom( dOrder );
      
   if( dOrder != m_currentOrder )
   {
      if( m_currentOrder > 0 )
      {
         if( m_pPolyWidget->layout( ) )
            delete m_pPolyWidget->layout( );
         
         m_labels.setAutoDelete( true );
         m_labels.clear( );
         m_labels.setAutoDelete( false );
         m_edits.setAutoDelete( true );
         m_edits.clear( );
         m_edits.setAutoDelete( false );
      }

      int nedit = polynom.count( );
      int nr = ( nedit + 2 ) / 3;
      int i;
      
      QGridLayout* gl = new QGridLayout( m_pPolyWidget, nr * 2 - 1, 9, 0 );
      QLabel* l = 0;
      PMFloatEdit* edit;
      PMFormulaLabel* fl;
      
      QValueList<PMPolynomExponents>::ConstIterator it = polynom.begin( );
      QString text;
      int row, col;
      QString plus( "+" );
      
      for( i = 0; it != polynom.end( ); i++, ++it )
      {
         row = ( i / 3 ) * 2;
         col = ( i % 3 ) * 3;

         if( i != 0 )
         {
            l = new QLabel( plus, m_pPolyWidget );
            m_labels.append( l );
            gl->addWidget( l, row, col );
            l->show( );
         }
         
         edit = new PMFloatEdit( m_pPolyWidget );
         connect( edit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
         m_edits.append( edit );
         gl->addWidget( edit, row, col + 1 );
         edit->show( );
         edit->setReadOnly( m_readOnly );

         fl = new PMFormulaLabel( *it, m_pPolyWidget );
         
         m_labels.append( fl );
         gl->addWidget( fl, row, col + 2 );
         fl->show( );
      }
      for( i = 0; i < ( nr - 1 ); i++ )
         gl->addRowSpacing( i * 2 + 1, KDialog::spacingHint( ) );
      
      emit sizeChanged( );
   }
   m_currentOrder = dOrder;

   
   if( dOrder == cOrder )
   {
      QPtrListIterator<PMFloatEdit> eit( m_edits );
      int i;
      for( i = 0; *eit; ++eit, ++i )
         ( *eit )->setValue( co[i] );
   }
   else if( dOrder > cOrder )
   {
      QValueList<PMPolynomExponents>::ConstIterator dit = polynom.begin( );
      QValueList<PMPolynomExponents>& cpoly
         = PMPolynomExponents::polynom( cOrder );
      QValueList<PMPolynomExponents>::ConstIterator cit = cpoly.begin( );
      QPtrListIterator<PMFloatEdit> eit( m_edits );
      int i = 0;
      
      for( ; ( dit != polynom.end( ) ) && ( cit != cpoly.end( ) ); ++dit, ++eit )
      {
         if( *dit == *cit )
         {
            ( *eit )->setValue( co[i] );
            i++;
            cit++;
         }
         else
            ( *eit )->setValue( 0.0 );
      }
      if( ( dit != polynom.end( ) ) || ( cit != cpoly.end( ) ) )
         kdError( PMArea ) << "displayExponents is buggy!\n";
   } 
   else // dOrder < cOrder
   {
      QValueList<PMPolynomExponents>::ConstIterator dit = polynom.begin( );
      QValueList<PMPolynomExponents>& cpoly
         = PMPolynomExponents::polynom( cOrder );
      QValueList<PMPolynomExponents>::ConstIterator cit = cpoly.begin( );
      QPtrListIterator<PMFloatEdit> eit( m_edits );
      int i = 0;
      
      for( ; ( dit != polynom.end( ) ) && ( cit != cpoly.end( ) ); ++cit, ++i )
      {
         if( *dit == *cit )
         {
            ( *eit )->setValue( co[i] );
            ++eit;
            ++dit;
         }
      }
      if( ( dit != polynom.end( ) ) || ( cit != cpoly.end( ) ) )
         kdError( PMArea ) << "displayExponents is buggy!\n";
   }
}

PMVector PMPolynomEdit::coefficients( ) const
{
   QPtrListIterator<PMFloatEdit> eit( m_edits );
   int num = m_edits.count( );
   PMVector v( num );
   int i;
   
   for( i = 0 ; *eit; ++eit, ++i )
      v[i] = ( *eit )->value( );
   return v;
}

void PMPolynomEdit::saveContents( )
{
   if( m_pDisplayedObject )
   {
      m_pDisplayedObject->setPolynomOrder( m_pOrder->value( ) );
      m_pDisplayedObject->setCoefficients( coefficients( ) );
      
      Base::saveContents( );

      m_pDisplayedObject->setSturm( m_pSturm->isChecked( ) );
   }
}

bool PMPolynomEdit::isDataValid( )
{
   QPtrListIterator<PMFloatEdit> eit( m_edits );
   for( ; *eit; ++eit )
      if( !( *eit )->isDataValid( ) )
         return false;
   return Base::isDataValid( );
}

void PMPolynomEdit::slotOrderChanged( int order )
{
   if( order == 2 )
      m_pSturm->hide( );
   else
      m_pSturm->show( );
   
   displayCoefficients( coefficients( ), m_currentOrder, order );
}

#include "pmpolynomedit.moc"
