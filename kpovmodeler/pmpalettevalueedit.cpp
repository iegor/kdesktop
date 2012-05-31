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


#include "pmpalettevalueedit.h"
#include "pmpalettevalue.h"
#include "pmlineedits.h"
#include "pmdebug.h"

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

PMPaletteValueEdit::PMPaletteValueEdit( QWidget* parent, const char* name ) 
      : QWidget( parent, name )
{
   QLabel* lbl;
   QHBoxLayout* layout;

   m_pIndexEdit = new PMIntEdit( this );
   m_pValueEdit = new PMFloatEdit( this );

   layout = new QHBoxLayout( this );
   lbl = new QLabel( i18n( "Index" ), this );
   layout->addWidget( lbl );
   layout->addSpacing( KDialog::spacingHint( ) );
   layout->addWidget( m_pIndexEdit );
   layout->addSpacing( KDialog::spacingHint( ) );
   lbl = new QLabel( i18n( "Value" ), this );
   layout->addWidget( lbl );
   layout->addSpacing( KDialog::spacingHint( ) );
   layout->addWidget( m_pValueEdit );

   connect( m_pIndexEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pValueEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
}

void PMPaletteValueEdit::setIndex( int idx )
{
   m_pIndexEdit->setValue( idx );
}

void PMPaletteValueEdit::setValue( double val )
{
   m_pValueEdit->setValue( val );
}

int PMPaletteValueEdit::index( )
{
   return m_pIndexEdit->value( );
}

double PMPaletteValueEdit::value( )
{
   return m_pValueEdit->value( );
}

void PMPaletteValueEdit::setReadOnly( bool yes )
{
   m_pIndexEdit->setReadOnly( yes );
   m_pValueEdit->setReadOnly( yes );
}
   
void PMPaletteValueEdit::setEnabled( bool yes )
{
   m_pIndexEdit->setEnabled( yes );
   m_pValueEdit->setEnabled( yes );
}

bool PMPaletteValueEdit::isDataValid( )
{
   return ( m_pIndexEdit->isDataValid( ) && m_pValueEdit->isDataValid( ) );
}

#include "pmpalettevalueedit.moc"
