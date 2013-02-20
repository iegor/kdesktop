/***************************************************************************
                          kquestion.cpp  -  description
                             -------------------
    begin                : Tue May 22 2001
    copyright            : (C) 2001 by Javier Campos Morales
    email                : javi@asyris.org
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "kquestion.h"
#include "kquestion.moc"

#include <qlayout.h>
#include <qpixmap.h>
#include <qvbox.h>
#include <qtimer.h>

#include <kprogress.h>
#include <klocale.h>

KQuestion::KQuestion( QWidget *parent, const char *name )
  : QHBox( parent, name ), _countdownTimer( 0 )
{
  initGUI();
}

KQuestion::~KQuestion()
{
}

/** Init graphical interface */
void KQuestion::initGUI()
{
  setFrameShape( QFrame::Box );
  setFrameShadow( QFrame::Plain );
  setPalette( QPalette( Qt::white ) );

  QVBox *picBox = new QVBox( this );

  _picture = new QLabel( picBox, "PixmapLabel1" );
  _picture->setScaledContents( FALSE );
  _picture->setPalette( QPalette( Qt::white ) );

  _countdownWidget = new KProgress( picBox );
  _countdownWidget->setSizePolicy( QSizePolicy( QSizePolicy::Preferred,
                                                QSizePolicy::Preferred ) );
  _countdownWidget->setFormat( i18n( "%v seconds left" ) );

    _view = new QTextEdit( this, "TextView1" );
    _view->setReadOnly( true );
    _view->setFrameShape( QTextEdit::NoFrame );
    _view->setHScrollBarMode( QTextEdit::AlwaysOff );
    _view->setTextFormat( QTextEdit::RichText );
    _view->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, true ) );
}

/** Set text */
void KQuestion::setText( const QString &text)
{
    _view->setText( text );
}

/** Set pixmap */
void KQuestion::setPixmap( const QPixmap pixmap)
{
    _picture->setPixmap( pixmap );
}

void KQuestion::countdown(int starttime)
{
  _totalCount = starttime;
  _currentCount = starttime;
  if (starttime > 0)
  {
    if (!_countdownTimer)
    {
      _countdownTimer = new QTimer(this);
      connect(_countdownTimer, SIGNAL(timeout()),
              this, SLOT(countDownOne()));
    }
    _countdownTimer->start(1000);
    // make one step less than time passes by in seconds
    // so that the user will see when time is up
    _countdownWidget->setTotalSteps(starttime);
    _countdownWidget->setProgress(starttime);
  }else{
    _countdownTimer->stop();
    _countdownWidget->reset();
  }
}

void KQuestion::countDownOne()
{
  _currentCount--;
  //_countdownWidget->setProgress(_totalCount - _currentCount);
  _countdownWidget->advance(-1);

  if (_currentCount == 0)
    _countdownTimer->stop();
}

void KQuestion::countdownVisible(bool visible)
{
  if (visible)
    _countdownWidget->show();
  else
    _countdownWidget->hide();
}

/** Get current time */
int KQuestion::getCurrentTime()
{
  return _totalCount - _currentCount;
}
