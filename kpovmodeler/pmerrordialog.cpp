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


#include "pmerrordialog.h"
#include "pmerrorflags.h"

#include <klocale.h>
#include <qlabel.h>
#include <qtextedit.h>

#include <kconfig.h>


QSize PMErrorDialog::s_size = QSize( 150, 200 );

PMErrorDialog::PMErrorDialog( const PMMessageList& messages, int errorFlags,
                              QWidget* parent, const char* name )
      : KDialogBase( parent, name, true, i18n( "Messages" ),
                     Ok | Cancel, Cancel )
{
   QVBox* page = makeVBoxMainWidget( );
   QLabel* text = new QLabel( QString( "" ), ( QWidget* )page );

   
   m_pTextView = new QTextEdit( ( QWidget* )page );
   m_pTextView->setReadOnly( true );

   m_messageDict.setAutoDelete( true );
   m_messages.setAutoDelete( true );
   
   PMMessageList::ConstIterator it;
   for( it = messages.begin( ); it != messages.end( ); ++it )
      m_messages.append( new PMMessage( *it ) );

   QPtrListIterator<PMMessage> pit( m_messages );
   for( ; pit.current( ); ++pit )
   {
      PMObject* obj = pit.current( )->linkedObject( );
      for( ; obj; obj = obj->parent( ) )
      {
         QPtrList<PMMessage>* pList = m_messageDict.find( obj );
         if( !pList )
         {
            pList = new QPtrList<PMMessage>;
            m_messageDict.insert( obj, pList );
         }
         pList->append( pit.current( ) );
         if( !obj->parent( ) )
         {
            if( obj->type( ) != "scene" )
            {
               kdError( PMArea ) << "A message contains an object that is not inserted in the scene" << endl;
               pit.current( )->setLinkedObject( 0 );
            }
         }
      }
   }
   
   displayMessages( );

   if( errorFlags & PMEWarning )
   {
      if( ( errorFlags & PMEError ) || ( errorFlags & PMEFatal ) )
         text->setText( i18n( "There were warnings and errors:" ) );
      else
         text->setText( i18n( "There were warnings:" ) );
   }
   else
      text->setText( i18n( "There were errors:" ) );
   
   setButtonOKText( KStdGuiItem::ok().text(),
                    i18n( "Proceed" ),
                    i18n( "When clicking <b>Proceed</b>, the program\n"
                          "will try to proceed with the current action." ) );
   setButtonCancelText( KStdGuiItem::cancel().text(),
                        i18n( "&Cancel" ),
                        i18n( "When clicking <b>Cancel<b>, the program\n"
                              "will cancel the current action." ) );
      
   if( errorFlags & PMEFatal )
      showButtonOK( false );
   else
      new QLabel( i18n( "Still try to proceed?" ), ( QWidget* )page );

   resize( s_size );
}

void PMErrorDialog::displayMessages( )
{
   QPtrListIterator<PMMessage> pit( m_messages );
   QString text;

   text = "<qt>\n";
   
   for( ; pit.current( ); ++pit )
      text += "<p>" + pit.current( )->text( ) + "</p>\n";

   text += "</qt>";
   
   m_pTextView->setText( text );
}

void PMErrorDialog::saveConfig( KConfig* cfg )
{
   cfg->setGroup( "Appearance" );
   cfg->writeEntry( "ErrorDialogSize", s_size );
}

void PMErrorDialog::restoreConfig( KConfig* cfg )
{
   cfg->setGroup( "Appearance" );

   QSize defaultSize( 150, 200 );
   s_size = cfg->readSizeEntry( "ErrorDialogSize", &defaultSize );
}

void PMErrorDialog::resizeEvent( QResizeEvent* ev )
{
   s_size = ev->size( );
}
#include "pmerrordialog.moc"
