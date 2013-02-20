/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2001 by Andreas Zehender
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

#include "pmlineedits.h"
#include <kmessagebox.h>
#include <klocale.h>

PMFloatEdit::PMFloatEdit( QWidget* parent, const char* name /*= 0*/ )
      : QLineEdit( parent, name )
{
   m_bCheckLower = false;
   m_bCheckUpper = false;
   m_lowerValue = 0;
   m_upperValue = 0;
   m_lowerOp = OpGreaterEqual;
   m_upperOp = OpLessEqual;

   connect( this, SIGNAL( textChanged( const QString& ) ),
            SLOT( slotEditTextChanged( const QString& ) ) );
}

void PMFloatEdit::setValidation( bool checkLower, double lowerValue,
                                 bool checkUpper, double upperValue )
{
   m_bCheckLower = checkLower;
   m_bCheckUpper = checkUpper;
   m_lowerValue = lowerValue;
   m_upperValue = upperValue;
}

void PMFloatEdit::setValidationOperator( ValidationOp l, ValidationOp u )
{
   m_lowerOp = l;
   m_upperOp = u;
}

bool PMFloatEdit::isDataValid( )
{
   bool ok = true;
   double d;
   d = text( ).toDouble( &ok );
   
   if( ok )
   {
      if( m_bCheckLower )
         ok = ok && ( m_lowerOp == OpGreaterEqual ?
                      d >= m_lowerValue : d > m_lowerValue );
      if( m_bCheckUpper )
         ok = ok && ( m_upperOp == OpLessEqual ?
                      d <= m_upperValue : d < m_upperValue );
      if( !ok )
      {
         if( m_bCheckLower && m_bCheckUpper )
            KMessageBox::error( this, i18n( "Please enter a float value "
                                            "between %1 and %2" )
                                .arg( m_lowerValue ).arg( m_upperValue ),
                                i18n( "Error" ) );
         else if( m_bCheckLower )
         {
            if( m_lowerOp == OpGreaterEqual )
               KMessageBox::error( this, i18n( "Please enter a float value "
                                               ">= %1" ).arg( m_lowerValue ),
                                   i18n( "Error" ) );
            else
               KMessageBox::error( this, i18n( "Please enter a float value "
                                               "> %1" ).arg( m_lowerValue ),
                                   i18n( "Error" ) );
         }
         else
         {
            if( m_upperOp == OpLessEqual )
               KMessageBox::error( this, i18n( "Please enter a float value "
                                               "<= %1" ).arg( m_upperValue ),
                                   i18n( "Error" ) );
            else
               KMessageBox::error( this, i18n( "Please enter a float value "
                                               "< %1" ).arg( m_upperValue ),
                                   i18n( "Error" ) );
         }
      }
   }
   else
   {
      KMessageBox::error( this, i18n( "Please enter a valid float value!" ),
                          i18n( "Error" ) );
   }

   if( !ok )
   {
      setFocus( );
      selectAll( );
   }
   return ok;
}

double PMFloatEdit::value( ) const
{
   return text( ).toDouble( );
}

void PMFloatEdit::setValue( double d, int precision )
{
   QString str;

   str.setNum( d, 'g', precision );
   setText( str );
}

void PMFloatEdit::slotEditTextChanged( const QString& /*t*/ )
{
   emit dataChanged( );
}





PMIntEdit::PMIntEdit( QWidget* parent, const char* name /*= 0*/ )
      : QLineEdit( parent, name )
{
   m_bCheckLower = false;
   m_bCheckUpper = false;
   m_lowerValue = 0;
   m_upperValue = 0;

   connect( this, SIGNAL( textChanged( const QString& ) ),
            SLOT( slotEditTextChanged( const QString& ) ) );
}

void PMIntEdit::setValidation( bool checkLower, int lowerValue,
                                 bool checkUpper, int upperValue )
{
   m_bCheckLower = checkLower;
   m_bCheckUpper = checkUpper;
   m_lowerValue = lowerValue;
   m_upperValue = upperValue;
}

bool PMIntEdit::isDataValid( )
{
   bool ok = true;
   int i;
   double d;
   
   i = text( ).toInt( &ok );
   if( !ok )
   {
      d = text( ).toDouble( &ok );
      if( ok )
      {
         i = ( int ) d;
         QString str;
         bool b = signalsBlocked( );
         blockSignals( true );
         str.setNum( i );
         setText( str );
         blockSignals( b );
      }
   }
   
   if( ok )
   {
      if( m_bCheckLower )
         ok = ok && ( i >= m_lowerValue );
      if( m_bCheckUpper )
         ok = ok && ( i <= m_upperValue );
      if( !ok )
      {
         if( m_bCheckLower && m_bCheckUpper )
            KMessageBox::error( this, i18n( "Please enter an integer value "
                                            "between %1 and %2" )
                                .arg( m_lowerValue ).arg( m_upperValue ),
                                i18n( "Error" ) );
         else if( m_bCheckLower )
            KMessageBox::error( this, i18n( "Please enter an integer value "
                                            ">= %1" ).arg( m_lowerValue ),
                                i18n( "Error" ) );
         else
            KMessageBox::error( this, i18n( "Please enter an integer value "
                                            "<= %1" ).arg( m_upperValue ),
                                i18n( "Error" ) );
      }
   }
   else
   {
      KMessageBox::error( this, i18n( "Please enter a valid integer value!" ),
                          i18n( "Error" ) );
   }

   if( !ok )
   {
      setFocus( );
      selectAll( );
   }
   return ok;
}

int PMIntEdit::value( ) const
{
   return text( ).toInt( );
}

void PMIntEdit::setValue( int i )
{
   QString str;

   str.setNum( i );
   setText( str );
}

void PMIntEdit::slotEditTextChanged( const QString& /*t*/ )
{
   emit dataChanged( );
}
#include "pmlineedits.moc"
