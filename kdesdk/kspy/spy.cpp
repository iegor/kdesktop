/***************************************************************************
                          spy.cpp  -  description
                             -------------------
    begin                : Tue May  1 02:59:33 BST 2001
    copyright            : (C) 2001 by Richard Moore
    email                : rich@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <klistviewsearchline.h>
#include <klocale.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qsplitter.h>
#include <ktabwidget.h>
#include <qvbox.h>

#include "navview.h"
#include "propsview.h"
#include "sigslotview.h"
#include "receiversview.h"
#include "classinfoview.h"
#include "spy.h"

extern "C"
{
  KDE_EXPORT void* init_libkspy()
  {
    qWarning( "KSpy: Initialising...\n" );
    Spy *s = new Spy();
    s->show();

    return 0;
  }
}

Spy::Spy( QWidget *parent, const char *name )
  : QWidget( parent, name )
{
  QVBoxLayout *layout = new QVBoxLayout( this, 11, 6 );

  QSplitter *div = new QSplitter( this );
  layout->addWidget( div );

  QVBox *leftPane = new QVBox( div );

  KListViewSearchLine *searchLine = new KListViewSearchLine( leftPane, "search line" );
  mNavView = new NavView( leftPane, "navigation view" );
  searchLine->setListView( mNavView );

  KTabWidget *tabs = new KTabWidget( div );

  mPropsView = new PropsView( tabs, "properties view" );
  tabs->addTab( mPropsView, i18n( "Properties" ) );

  mSigSlotView = new SigSlotView( tabs, "signals and slots view" );
  tabs->addTab( mSigSlotView, i18n( "Signals && Slots" ) );

  mReceiversView = new ReceiversView( tabs, "receivers view" );
  tabs->addTab( mReceiversView, i18n( "Receivers" ) );

  mClassInfoView = new ClassInfoView( tabs, "class info view" );
  tabs->addTab( mClassInfoView, i18n( "Class Info" ) );

  mNavView->buildTree();

  connect( mNavView, SIGNAL( selected( QObject * ) ),
           mPropsView, SLOT( setTarget( QObject * ) ) );
  connect( mNavView, SIGNAL( selected( QObject * ) ),
           mSigSlotView, SLOT( setTarget( QObject * ) ) );
  connect( mNavView, SIGNAL( selected( QObject * ) ),
           mReceiversView, SLOT( setTarget( QObject * ) ) );
  connect( mNavView, SIGNAL( selected( QObject * ) ),
           mClassInfoView, SLOT( setTarget( QObject * ) ) );
}

Spy::~Spy()
{
}


void Spy::setTarget( QWidget *target )
{
  mTarget = target;
  mPropsView->buildList( mTarget );
  mSigSlotView->buildList( mTarget );
  mReceiversView->buildList( mTarget );
  mClassInfoView->buildList( mTarget );
}

void Spy::keyPressEvent( QKeyEvent *event )
{
  if ( event->key() == Qt::Key_Up ) {
    event->accept();
    QApplication::postEvent( mNavView, new QKeyEvent( QEvent::KeyPress, Qt::Key_Up, 0, 0 ) );
  } else if ( event->key() == Qt::Key_Down ) {
    event->accept();
    QApplication::postEvent( mNavView, new QKeyEvent( QEvent::KeyPress, Qt::Key_Down, 0, 0 ) );
  } else if ( event->key() == Qt::Key_Return ) {
    event->accept();
    mNavView->expandVisibleTree();
  }
}

#include "spy.moc"
