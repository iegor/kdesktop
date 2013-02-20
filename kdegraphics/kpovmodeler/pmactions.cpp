/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2002 by Andreas Zehender
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

#include "pmactions.h"

#include <qcombobox.h>
#include <qwhatsthis.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qstyle.h>
#include <qpainter.h>
#include <ktoolbar.h>
#include <ktoolbarbutton.h>

#include "pmdebug.h"

// Fixed widths are calculated wrong in a toolbar.
// Fixed sizeHint for the combo box to return
// at least the minimum size
class PMComboBox : public QComboBox
{
public:
   PMComboBox( QWidget* parent, const char* name = 0 )
         : QComboBox( parent, name )
   {
   }

   virtual QSize minimumSizeHint( ) const
   {
      QSize s = QComboBox::minimumSizeHint( );
      return s.expandedTo( minimumSize( ) );
   }
   virtual QSize sizeHint( ) const
   {
      QSize s = QComboBox::sizeHint( );
      return s.expandedTo( minimumSize( ) );
   }
};

PMComboAction::PMComboAction( const QString& text, int accel, const QObject* receiver, const char* member,
                              QObject* parent, const char* name )
      : KAction( text, accel, parent, name )
{
   m_receiver = receiver;
   m_member = member;
   m_minWidth = 0;
   m_maxWidth = 0;
}

PMComboAction::~PMComboAction( )
{
}

int PMComboAction::plug( QWidget* w, int index )
{
   if( !w->inherits( "KToolBar" ) )
      return -1;

   KToolBar* toolBar = ( KToolBar* ) w;

   int id = KAction::getToolButtonID( );

   QComboBox* comboBox = new PMComboBox( toolBar );
   if( m_minWidth > 0 )
      comboBox->setMinimumWidth( m_minWidth );
   if( m_maxWidth > 0 )
      comboBox->setMaximumWidth( m_maxWidth );

   toolBar->insertWidget( id, m_minWidth > 0 ? m_minWidth : 300,
                          comboBox, index );
   connect( comboBox, SIGNAL( activated( int ) ), m_receiver, m_member );

   addContainer( toolBar, id );

   connect( toolBar, SIGNAL( destroyed( ) ), this, SLOT( slotDestroyed( ) ) );

   //toolBar->setItemAutoSized( id, true );

   m_combo = comboBox;

   emit plugged( );

   QWhatsThis::add( comboBox, whatsThis( ) );

   return containerCount( ) - 1;
}

void PMComboAction::unplug( QWidget *w )
{
   if( !w->inherits( "KToolBar" ) )
      return;

   KToolBar *toolBar = ( KToolBar* ) w;

   int idx = findContainer( w );

   toolBar->removeItem( itemId( idx ) );

   removeContainer( idx );
   m_combo = 0L;
}


// Use a toolbutton instead of a label so it is styled correctly.
// copied from konq_actions.cc
class PMToolBarLabel : public QToolButton
{
public:
   PMToolBarLabel( const QString& text, QWidget* parent = 0, const char* name = 0 )
         : QToolButton( parent, name )
   {
      setText( text );
   }
protected:
   QSize sizeHint( ) const
   {
      int w = fontMetrics( ).width( text( ) );
      int h = fontMetrics( ).height( );
      return QSize( w, h );
   }
   void drawButton( QPainter* p )
   {
#if ( QT_VERSION >= 300 )
      // Draw the background
      style( ).drawComplexControl( QStyle::CC_ToolButton, p, this, rect( ), colorGroup( ),
                                   QStyle::Style_Enabled, QStyle::SC_ToolButton );
      // Draw the label
      style( ).drawControl( QStyle::CE_ToolButtonLabel, p, this, rect( ), colorGroup( ),
                            QStyle::Style_Enabled );
#else
      p->drawText( rect( ), Qt::AlignVCenter | Qt::AlignLeft, text( ) );
#endif      
   }
};

PMLabelAction::PMLabelAction( const QString &text, QObject *parent, const char *name )
    : KAction( text, 0, parent, name )
{
   m_button = 0;
}

int PMLabelAction::plug( QWidget *widget, int index )
{
   //do not call the previous implementation here

   if( widget->inherits( "KToolBar" ) )
   {
      KToolBar* tb = ( KToolBar* ) widget;

      int id = KAction::getToolButtonID( );

      m_button = new PMToolBarLabel( text( ), widget );
      tb->insertWidget( id, m_button->width( ), m_button, index );

      addContainer( tb, id );

      connect( tb, SIGNAL( destroyed( ) ), this, SLOT( slotDestroyed( ) ) );

      return containerCount( ) - 1;
  }

  return -1;
}

void PMLabelAction::unplug( QWidget *widget )
{
   if( widget->inherits( "KToolBar" ) )
   {
      KToolBar* bar = ( KToolBar* ) widget;
      
      int idx = findContainer( bar );
      
      if( idx != -1 )
      {
         bar->removeItem( itemId( idx ) );
         removeContainer( idx );
      }
      
      m_button = 0;
      return;
   }
}


PMSpinBoxAction::PMSpinBoxAction( const QString& text, int accel, const QObject* receiver, const char* member,
                                  QObject* parent, const char* name )
      : KAction( text, accel, parent, name )
{
   m_receiver = receiver;
   m_member = member;
}

PMSpinBoxAction::~PMSpinBoxAction( )
{
}

int PMSpinBoxAction::plug( QWidget* w, int index )
{
   if( !w->inherits( "KToolBar" ) )
      return -1;

   KToolBar* toolBar = ( KToolBar* ) w;

   int id = KAction::getToolButtonID( );
   
   QSpinBox* spinBox = new QSpinBox( -1000, 1000, 1, w );
   toolBar->insertWidget( id, 70, spinBox, index );
   
   connect( spinBox, SIGNAL( valueChanged( int ) ), m_receiver, m_member );

   addContainer( toolBar, id );

   connect( toolBar, SIGNAL( destroyed( ) ), this, SLOT( slotDestroyed( ) ) );
   //toolBar->setItemAutoSized( id, false );

   m_spinBox = spinBox;

   emit plugged( );

   QWhatsThis::add( spinBox, whatsThis( ) );

   return containerCount( ) - 1;
}

void PMSpinBoxAction::unplug( QWidget *w )
{
   if( !w->inherits( "KToolBar" ) )
      return;

   KToolBar *toolBar = (KToolBar *)w;

   int idx = findContainer( w );

   toolBar->removeItem( itemId( idx ) );

   removeContainer( idx );
   m_spinBox = 0L;
}

#include "pmactions.moc"
