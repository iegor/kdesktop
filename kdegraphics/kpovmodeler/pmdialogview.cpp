/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2003 by Andreas Zehender
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


#include "pmdialogview.h"
#include "pmdialogeditbase.h"
#include "pmallobjects.h"
#include "pmdebug.h"
#include "pmfactory.h"
#include "pmdatachangecommand.h"
#include "pmpart.h"
#include "pmdocumentationmap.h"

#include <klocale.h>
#include <kmessagebox.h>
#include <kdialog.h>
#include <kprocess.h>
#include <kiconloader.h>
#include <krun.h>
#include <kstdguiitem.h>

#include <qlayout.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qscrollview.h>
#include <qlabel.h>

PMDialogEditContent::PMDialogEditContent( QWidget* parent, const char* name )
      : QScrollView( parent, name )
{
   m_pContents = 0;
   setVScrollBarMode( AlwaysOff );
   setHScrollBarMode( AlwaysOff );
   setFrameStyle( Panel | Sunken );
   setLineWidth( 1 );
   setResizePolicy( Manual );
}

void PMDialogEditContent::setContents( QWidget* wid )
{
   if( m_pContents )
      removeChild( m_pContents );

   m_pContents = wid;

   if( m_pContents )
   {
      addChild( m_pContents );
      calculateSize( );
   }
}

void PMDialogEditContent::calculateSize( )
{
   int fw = lineWidth( ) * 2;
   if( m_pContents )
   {
      QSize newSize = m_pContents->minimumSizeHint( );

      setVScrollBarMode( AlwaysOff );
      setHScrollBarMode( AlwaysOff );
      setMargins( 0, 0, 0, 0 );

      if( width( ) - fw < newSize.width( ) )
      {
         setHScrollBarMode( AlwaysOn );

         if( ( height( ) - horizontalScrollBar( )->height( ) - fw )
             < newSize.height( ) )
            setVScrollBarMode( AlwaysOn );
         else
            newSize.setHeight( height( ) - horizontalScrollBar( )->height( ) - 2 );
      }
      else if( height( ) - fw < newSize.height( ) )
      {
         setVScrollBarMode( AlwaysOn );

         if( ( width( ) - verticalScrollBar( )->width( ) - fw )
             < newSize.width( ) )
            setHScrollBarMode( AlwaysOn );
         else
            newSize.setWidth( width( ) - verticalScrollBar( )->width( ) - fw );
      }
      else
      {
         newSize.setWidth( width( ) - fw );
         newSize.setHeight( height( ) - fw );
      }

      resizeContents( newSize.width( ), newSize.height( ) );
      m_pContents->resize( newSize );
   }
}

void PMDialogEditContent::resizeEvent( QResizeEvent* /* ev */ )
{
   calculateSize( );
}

PMDialogView::PMDialogView( PMPart* part, QWidget* parent, const char* name )
      : PMViewBase( parent, name )
{
   m_pDisplayedWidget = 0;
   m_unsavedData = false;
   m_pHelper = new PMDialogEditContent( this );
   m_pHelper->show( );
   m_pPart = part;

   m_pLayout = new QVBoxLayout( this, KDialog::marginHint( ),
                                KDialog::spacingHint( ) );

   QHBoxLayout* labelLayout = new QHBoxLayout( m_pLayout );
   m_pPixmapLabel = new QLabel( this );
   m_pObjectTypeLabel = new QLabel( this );
   labelLayout->addWidget( m_pPixmapLabel );
   labelLayout->addWidget( m_pObjectTypeLabel );
   labelLayout->addStretch( );

   m_pLayout->addWidget( m_pHelper, 2 );
   m_pLayout->addStretch( );

   QHBoxLayout* buttonLayout = new QHBoxLayout( m_pLayout );
   m_pHelpButton = new KPushButton( KStdGuiItem::help(), this );
   buttonLayout->addWidget( m_pHelpButton );
   connect( m_pHelpButton, SIGNAL( clicked( ) ), this, SLOT( slotHelp( ) ) );
   m_pHelpButton->setEnabled( false );

   m_pApplyButton = new KPushButton( KStdGuiItem::apply(), this );
   buttonLayout->addWidget( m_pApplyButton );
   connect( m_pApplyButton, SIGNAL( clicked( ) ), this, SLOT( slotApply( ) ) );
   m_pApplyButton->setEnabled( false );

   buttonLayout->addStretch( );

   m_pCancelButton = new KPushButton( KStdGuiItem::cancel(), this );
   buttonLayout->addWidget( m_pCancelButton );
   connect( m_pCancelButton, SIGNAL( clicked( ) ), this, SLOT( slotCancel( ) ) );
   m_pCancelButton->setEnabled( false );

   m_pLayout->activate( );

   connect( part, SIGNAL( refresh( ) ), SLOT( slotRefresh( ) ) );
   connect( part, SIGNAL( objectChanged( PMObject*, const int, QObject* ) ),
            SLOT( slotObjectChanged( PMObject*, const int, QObject* ) ) );
   connect( part, SIGNAL( clear( ) ), SLOT( slotClear( ) ) );
   connect( part, SIGNAL( aboutToRender( ) ), SLOT( slotAboutToRender( ) ) );
   connect( part, SIGNAL( aboutToSave( ) ), SLOT( slotAboutToRender( ) ) );
   connect( this, SIGNAL( objectChanged( PMObject*, const int, QObject* ) ),
            part, SLOT( slotObjectChanged( PMObject*, const int, QObject* ) ) );

   displayObject( m_pPart->activeObject( ) );
}



PMDialogView::~PMDialogView( )
{
   emit destroyed( this );
}

void PMDialogView::slotObjectChanged( PMObject* obj, const int mode, QObject* sender )
{
   if( sender == this )
      return;
   if( mode & PMCNewSelection )
   {
      if( m_pDisplayedWidget )
         if( m_pDisplayedWidget->displayedObject( ) )
            if( m_unsavedData )
               if( shouldSaveData( ) )
                  slotApply( );

      displayObject( obj );
   }
   if( mode & ( PMCSelected | PMCDeselected ) )
   {
      if( m_pDisplayedWidget )
         if( m_pDisplayedWidget->displayedObject( ) )
            if( m_unsavedData )
               if( shouldSaveData( ) )
                  slotApply( );

      displayObject( 0 );
   }
   if( mode & PMCRemove )
   {
      if( m_pDisplayedWidget )
      {
         if( m_pDisplayedWidget->displayedObject( ) == obj )
         {
            displayObject( 0 );
         }
      }
   }
   if( mode & ( PMCData | PMCDescription ) )
   {
      if( m_pDisplayedWidget )
      {
         if( m_pDisplayedWidget->displayedObject( ) )
         {
            if( m_pDisplayedWidget->displayedObject( ) == obj )
            {
               displayObject( obj, mode & PMCDescription );
               m_unsavedData = false;
               m_pApplyButton->setEnabled( false );
               m_pCancelButton->setEnabled( false );
            }
         }
      }
   }
   if( mode & PMCControlPointSelection )
   {
      if( m_pDisplayedWidget )
         m_pDisplayedWidget->updateControlPointSelection( );
   }
}

bool PMDialogView::shouldSaveData( )
{
   return ( KMessageBox::questionYesNo(
      this, i18n( "This object was modified.\n\nSave changes?" ),
      i18n( "Unsaved Changes" ), KStdGuiItem::save(), KStdGuiItem::discard() ) == KMessageBox::Yes );
}

void PMDialogView::slotRefresh( )
{
   if( m_pDisplayedWidget )
      m_pDisplayedWidget->redisplay( );
}

void PMDialogView::slotClear( )
{
   displayObject( 0 );
}

void PMDialogView::slotApply( )
{
   if( m_pDisplayedWidget )
   {
      PMObject* obj = m_pDisplayedWidget->displayedObject( );
      if( obj )
      {
         if( m_pDisplayedWidget->isDataValid( ) )
         {
            PMDataChangeCommand* cmd;

            obj->createMemento( );
            m_pDisplayedWidget->saveData( );
            cmd = new PMDataChangeCommand( obj->takeMemento( ) );
            m_pPart->executeCommand( cmd );

            m_pApplyButton->setEnabled( false );
            m_pCancelButton->setEnabled( false );
            m_unsavedData = false;
         }
      }
   }
}

void PMDialogView::slotHelp( )
{
   if( m_pDisplayedWidget && m_pDisplayedWidget->displayedObject( ) )
   {
      QString url = PMDocumentationMap::theMap( )->documentation(
         m_pDisplayedWidget->displayedObject( )->className( ) );
      if( !url.isEmpty( ) )
      {
         // Instead of calling invokeBrowser run konqueror directly.
         // invokeBrowser was ignoring html anchors.
         url = "konqueror " + KProcess::quote(url);
         KRun::runCommand( url );
      }
   }
}

void PMDialogView::slotCancel( )
{
   m_pCancelButton->setEnabled( false );
   m_pApplyButton->setEnabled( false );
   m_unsavedData = false;

   if( m_pDisplayedWidget )
   {
      bool b = m_pDisplayedWidget->signalsBlocked( );
      m_pDisplayedWidget->blockSignals( true );
      m_pDisplayedWidget->redisplay( );
      m_pDisplayedWidget->blockSignals( b );
      slotSizeChanged( );
   }
}

void PMDialogView::slotDataChanged( )
{
//   kdDebug( PMArea ) << "PMDialogView::slotDataChanged\n";
   m_pApplyButton->setEnabled( true ) ;
   m_pCancelButton->setEnabled( true );

   m_unsavedData = true;
}

void PMDialogView::slotSizeChanged( )
{
   // force recalculating of the layout
   if( m_pDisplayedWidget )
      if( m_pDisplayedWidget->layout( ) )
         m_pDisplayedWidget->layout( )->activate( );
   m_pHelper->calculateSize( );
}

void PMDialogView::slotControlPointSelectionChanged( )
{
   if( m_pDisplayedWidget && m_pDisplayedWidget->displayedObject( ) )
      emit objectChanged( m_pDisplayedWidget->displayedObject( ),
                          PMCControlPointSelection, this );
}

void PMDialogView::displayObject( PMObject* obj, bool updateDescription )
{
   PMDialogEditBase* old = 0;

   if( !obj )
   {
      if( m_pDisplayedWidget )
         old = m_pDisplayedWidget;

      m_pDisplayedWidget = new PMDialogEditBase( m_pHelper->viewport( ) );
      m_pDisplayedWidget->createWidgets( );
      m_pHelper->setContents( m_pDisplayedWidget );
      m_pPixmapLabel->setText( "" );
      m_pObjectTypeLabel->setText( "" );
   }
   else
   {
      bool newWidget = true;
      if( m_pDisplayedWidget )
      {
         if( m_pDisplayedWidget->displayedObject( ) )
         {
            if( obj->type( ) == m_pDisplayedWidget->displayedObject( )->type( ) )
            {
               // current widget displays object of the same type
               // reuse the widget
               newWidget = false;
            }
         }
      }

      if( newWidget )
      {
         // first create the new widget, then delete the old one.
         if( m_pDisplayedWidget )
            old = m_pDisplayedWidget;
         m_pDisplayedWidget = obj->editWidget( m_pHelper->viewport( ) );
         m_pDisplayedWidget->setPart( m_pPart );
         m_pDisplayedWidget->createWidgets( );
         m_pHelper->setContents( m_pDisplayedWidget );

         if( m_pDisplayedWidget )
         {
            connect( m_pDisplayedWidget, SIGNAL( dataChanged( ) ),
                     this, SLOT( slotDataChanged( ) ) );
            connect( m_pDisplayedWidget, SIGNAL( sizeChanged( ) ),
                     this, SLOT( slotSizeChanged( ) ) );
            connect( m_pDisplayedWidget, SIGNAL( aboutToRender( ) ),
                     this, SLOT( slotAboutToRender( ) ) );
            connect( m_pDisplayedWidget,
                     SIGNAL( controlPointSelectionChanged( ) ),
                     SLOT( slotControlPointSelectionChanged( ) ) );

         }
      }
   }

   if( m_pDisplayedWidget )
   {
      bool b = m_pDisplayedWidget->signalsBlocked( );
      m_pDisplayedWidget->blockSignals( true );
      m_pDisplayedWidget->displayObject( obj );
      m_pDisplayedWidget->blockSignals( b );

      m_pHelpButton->setEnabled( !m_pDisplayedWidget->helpTopic( ).isNull() );

      if( !m_pDisplayedWidget->isVisible( ) )
         m_pDisplayedWidget->show( );
      if( obj && updateDescription )
      {
         m_pPixmapLabel->setPixmap( SmallIcon( obj->pixmap( ), PMFactory::instance( ) ) );
         m_pObjectTypeLabel->setText( obj->description( ) );
      }
      slotSizeChanged( );
   }

   if( old )
      delete old;

   m_pApplyButton->setEnabled( false );
   m_pCancelButton->setEnabled( false );
   m_pHelpButton->setEnabled( !PMDocumentationMap::theMap( )
                              ->povrayDocumentationPath( ).isEmpty( ) );

   m_unsavedData = false;
}

void PMDialogView::keyPressEvent( QKeyEvent* ev )
{
   if( ( ev->key( ) == Key_Return ) || ( ev->key( ) == Key_Enter ) )
      slotApply( );
}

void PMDialogView::slotAboutToRender( )
{
   if( m_unsavedData )
      if( shouldSaveData( ) )
         slotApply( );
}

QString PMDialogView::description( ) const
{
   return i18n( "Object Properties" );
}

QString PMDialogViewFactory::description( ) const
{
   return i18n( "Object Properties" );
}

#include "pmdialogview.moc"
