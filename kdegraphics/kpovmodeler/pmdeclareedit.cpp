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


#include "pmdeclareedit.h"
#include "pmdeclare.h"
#include "pmpart.h"
#include "pmsymboltable.h"
#include "pmscanner.h"
#include "pmobjectselect.h"

#include <qlayout.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qtextstream.h>
#include <qpushbutton.h>
#include <qlistbox.h>

#include <klocale.h>
#include <kmessagebox.h>

PMDeclareEdit::PMDeclareEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
   m_pSelectedObject = 0;
}

void PMDeclareEdit::createTopWidgets( )
{
   Base::createTopWidgets( );

   QHBoxLayout* layout = new QHBoxLayout( topLayout( ) );
   m_pNameEdit = new QLineEdit( this );
   m_pNameEdit->setMaxLength( 40 );
   QLabel* label = new QLabel( i18n( "Identifier:" ), this );

   layout->addWidget( label );
   layout->addWidget( m_pNameEdit );

   connect( m_pNameEdit, SIGNAL( textChanged( const QString& ) ),
            SLOT( slotNameChanged( const QString& ) ) );
}

void PMDeclareEdit::createBottomWidgets( )
{
   QLabel* l = new QLabel( i18n( "Linked objects:" ), this );
   topLayout( )->addWidget( l );

   m_pLinkedObjects = new QListBox( this );
   m_pLinkedObjects->setMinimumHeight( 100 );
   connect( m_pLinkedObjects, SIGNAL( highlighted( QListBoxItem* ) ),
            SLOT( slotItemSelected( QListBoxItem* ) ) );
   topLayout( )->addWidget( m_pLinkedObjects, 1 );

   QHBoxLayout* layout = new QHBoxLayout( topLayout( ) );
   m_pSelectButton = new QPushButton( i18n( "Select..." ), this );
   m_pSelectButton->setEnabled( false );

   connect( m_pSelectButton, SIGNAL( clicked( ) ), SLOT( slotSelect( ) ) );
   layout->addStretch( );
   layout->addWidget( m_pSelectButton );

   Base::createBottomWidgets( );
}

void PMDeclareEdit::displayObject( PMObject* o )
{
   if( o->isA( "Declare" ) )
   {
      m_pDisplayedObject = ( PMDeclare* ) o;
      m_pNameEdit->setText( QString( m_pDisplayedObject->id( ) ) );

      m_pNameEdit->setReadOnly( m_pDisplayedObject->isReadOnly( ) );

      PMObjectListIterator it( m_pDisplayedObject->linkedObjects( ) );
      m_pLinkedObjects->clear( );

      for( ; it.current( ); ++it )
         m_pLinkedObjects->insertItem(
            new PMListBoxObject( it.current( ) ) );

      m_pSelectButton->setEnabled( false );
      m_pSelectedObject = 0;

      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMDeclareEdit: Can't display object\n";
}

void PMDeclareEdit::saveContents( )
{
   if( m_pDisplayedObject )
   {
      Base::saveContents( );
      m_pDisplayedObject->setID( m_pNameEdit->text( ) );
   }
}

bool PMDeclareEdit::isDataValid( )
{
   if( !Base::isDataValid( ) )
      return false;

   QString text = m_pNameEdit->text( );
   if( text.length( ) == 0 )
   {
      KMessageBox::error( this, i18n( "Please enter an identifier!" ),
                          i18n( "Error" ) );
      return false;
   }

   if( text == m_pDisplayedObject->id( ) )
      return true;

   QTextStream str( &text, IO_ReadOnly );
   QChar c;
   int ac;
   bool ok = true;
   int i = 0;
   while( !str.atEnd( ) && ok )
   {
      str >> c;
      ac = c.latin1( );
      // QChar::category can't be used because umlauts are not allowed
      if( i == 0 )
         ok = ( ( ( ac >= 'a' ) && ( ac <= 'z' ) ) ||
                ( ( ac >= 'A' ) && ( ac <= 'Z' ) ) ||
                ( ac == '_' ) );
      else
         ok  = ( ( ( ac >= 'a' ) && ( ac <= 'z' ) ) ||
                 ( ( ac >= 'A' ) && ( ac <= 'Z' ) ) ||
                 ( ( ac >= '0' ) && ( ac <= '9' ) ) ||
                 ( ac == '_' ) );
      i++;
   }
   if( !ok )
   {
      KMessageBox::error( this, i18n( "An identifier may consist of letters,"
                                      " digits and the underscore character"
                                      " ('_').\n"
                                      "The first character must be a letter"
                                      " or the underscore character!" ),
                          i18n( "Error" ) );
      return false;
   }
   // valid identifer!

   PMReservedWordDict* dict = PMScanner::reservedWords( );
   if( dict->find( text.latin1( ) ) != -1 )
   {
      KMessageBox::error( this, i18n( "You can't use a povray reserved word"
                                      " as an identifier!" ), i18n( "Error" ) );
      return false;
   }
   dict = PMScanner::directives( );
   if( dict->find( text.latin1( ) ) != -1 )
   {
      KMessageBox::error( this, i18n( "You can't use a povray directive"
                                      " as an identifier!" ), i18n( "Error" ) );
      return false;
   }

   // no reserved word
   PMSymbolTable* st = part( )->symbolTable( );
   if( st->find( text ) )
   {
      KMessageBox::error( this, i18n( "Please enter a unique identifier!" ),
                          i18n( "Error" ) );
      return false;
   }
   return true;
}

void PMDeclareEdit::slotNameChanged( const QString& )
{
   emit dataChanged( );
}

void PMDeclareEdit::slotItemSelected( QListBoxItem* item )
{
   m_pSelectedObject = ( ( PMListBoxObject* ) item )->object( );
   m_pSelectButton->setEnabled( true );
}

void PMDeclareEdit::slotSelect( )
{
   if( m_pSelectedObject )
      part( )->slotObjectChanged( m_pSelectedObject, PMCNewSelection, this );
}
#include "pmdeclareedit.moc"
