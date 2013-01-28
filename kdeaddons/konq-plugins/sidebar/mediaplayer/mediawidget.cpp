/***************************************************************************
                          mediawidget.cpp - The main widget
                             -------------------
    begin                : Sat June 23 13:35:30 CEST 2001
    copyright            : (C) 2001 Joseph Wenninger
    email                : jowenn@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "mediawidget.h"
#include "mediawidget.moc"
#include "player.h"

#include <kdebug.h>
#include <kurl.h>
#include <kurldrag.h>
#include <klocale.h>

#include <qlabel.h>
#include <qwidget.h>
#include <qpushbutton.h>
#include <qlcdnumber.h>
#include <qpopupmenu.h>
#include <qslider.h>
#include <qtooltip.h>

KSB_MediaWidget::KSB_MediaWidget(QWidget *parent):KSB_MediaWidget_skel(parent)
{
	player = new Player(this);
	empty();

	QFont labelFont = time->font();
	labelFont.setPointSize(18);
	labelFont.setBold(true);
	time->setFont(labelFont);

	connect(Play, SIGNAL(clicked()), player, SLOT(play()));
	connect(Pause, SIGNAL(clicked()), player, SLOT(pause()));
	connect(Stop, SIGNAL(clicked()), player, SLOT(stop()));

	connect(player, SIGNAL(timeout()), this, SLOT(playerTimeout()));
	connect(player, SIGNAL(finished()), this, SLOT(playerFinished()));
	connect(player, SIGNAL(playing()), this, SLOT(playing()));
	connect(player, SIGNAL(paused()), this, SLOT(paused()));
	connect(player, SIGNAL(stopped()), this, SLOT(stopped()));
	connect(player, SIGNAL(empty()), this, SLOT(empty()));

	connect(Position, SIGNAL(userChanged(int)), this, SLOT(skipToWrapper(int)));
	connect(this, SIGNAL(skipTo(unsigned long)), player, SLOT(skipTo(unsigned long)));
	setAcceptDrops(true);

	pretty="";
	needLengthUpdate=false;

	QToolTip::add(Play,i18n("Play"));
	QToolTip::add(Pause,i18n("Pause"));
	QToolTip::add(Stop,i18n("Stop"));
}

void KSB_MediaWidget::skipToWrapper(int second)
{
	emit skipTo((unsigned long)(second*1000));
}

void KSB_MediaWidget::dragEnterEvent ( QDragEnterEvent * e)
{
	e->accept(KURLDrag::canDecode(e));
}

void KSB_MediaWidget::dropEvent ( QDropEvent * e)
{
	m_kuri_list.clear();
	if (KURLDrag::decode(e, m_kuri_list))
	{
		playerFinished();
	}
}


void KSB_MediaWidget::playerTimeout()
{
	if(player->current().isEmpty())
		return;

	if(Position->currentlyPressed())
		return;

// update the scrollbar length
	if(player->getLength())
	{
		int range = (int)(player->getLength() / 1000);
		Position->setRange(0, range);
		if (needLengthUpdate)
		{
			int counter = player->lengthString().length() - (player->lengthString().find("/")+1);
			QString length=player->lengthString().right(counter);
			needLengthUpdate=false;
		}
	}
	else
	{
		Position->setRange(0, 1);
	}
	// set the position
	Position->setValue((int)(player->getTime() / 1000));

	// update the time label
	// catch files with duration > 99mins correctly
	time->setText(player->lengthString());
}

void KSB_MediaWidget::playerFinished()
{
	if( m_kuri_list.count() > 0 )
	{
		KURL kurl = m_kuri_list.first();
		m_kuri_list.remove( kurl );
		bool validFile = player->openFile( kurl );
		if (validFile) {
			currentFile->setText( kurl.fileName() );
			player->play();
			needLengthUpdate=true;
			pretty=kurl.prettyURL();
		} else {
			currentFile->setText( i18n("Not a sound file") );
			playerFinished();
		}
	}
}

void KSB_MediaWidget::playing()
{
	Play->setEnabled(false);
	Pause->setEnabled(true);
	Stop->setEnabled(true);
}

void KSB_MediaWidget::paused()
{
	Play->setEnabled(true);
	Pause->setEnabled(false);
	Stop->setEnabled(true);
}

void KSB_MediaWidget::stopped()
{
	Position->setValue(0);
	time->setText("00:00/00:00");
	Play->setEnabled(true);
	Pause->setEnabled(false);
	Stop->setEnabled(false);
}

void KSB_MediaWidget::empty()
{
	Position->setValue(0);
	time->setText("00:00/00:00");
	Play->setEnabled(false);
	Pause->setEnabled(false);
	Stop->setEnabled(false);
}
