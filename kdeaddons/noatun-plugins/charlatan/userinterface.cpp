/*
 * Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.	IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <noatun/scrollinglabel.h>
#include "userinterface.h"
#include <noatun/app.h>
#include <noatun/player.h>
#include <noatun/controls.h>
#include <noatun/effects.h>

#include <qaccel.h>
#include <qtoolbutton.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qframe.h>
#include <kurldrag.h>
#include <kstatusbar.h>
#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include <kurldrag.h>
#include <kiconloader.h>
#include <noatun/playlist.h>
#include <noatun/stdaction.h>

#include "configmodule.h"
#include "pixmaps.h"
#include "seeker.h"

Charlatan::Charlatan()
	: QWidget									(0, "Charlatan"),
		UserInterface						(),
		previousButton_					(0L),
		stopButton_							(0L),
		playButton_							(0L),
		nextButton_							(0L),
		playListToggleButton_		(0L),
		restartButton_					(0L),
		seekSlider_							(0L),
		titleLabel_							(0L),
		positionLabel_					(0L),
		showingVolumeTimer_			(0L),
		title_									("c.h.a.r.l.a.t.a.n")
{
	setAcceptDrops(true);

	showingVolumeTimer_ 	= new QTimer(this);

	connect
		(
			showingVolumeTimer_,
			SIGNAL(timeout()),
			SLOT(slotStopShowingVolume())
		);

	playListToggleButton_	= new QToolButton(this);
	previousButton_				= new QToolButton(this);
	nextButton_						= new QToolButton(this);
	stopButton_						= new QToolButton(this);
	restartButton_				= new QToolButton(this);
	playButton_						= new QToolButton(this);

	seekSlider_						= new Seeker(this);
	positionLabel_				= new QLabel(this);

	titleLabel_						= new ScrollingLabel(i18n("No File Loaded"), this);

	playListToggleButton_	->setPixmap(QPixmap((const char **)px_playlist));
	previousButton_				->setPixmap(QPixmap((const char **)px_previous));
	nextButton_						->setPixmap(QPixmap((const char **)px_next));
	stopButton_						->setPixmap(QPixmap((const char **)px_stop));
	restartButton_				->setPixmap(QPixmap((const char **)px_restart));
	playButton_						->setPixmap(QPixmap((const char **)px_play1));

	playListToggleButton_	->setToggleButton(true);

	QVBoxLayout * mainLayout		= new QVBoxLayout(this);

	mainLayout->addWidget(titleLabel_);

  mainLayout->addSpacing(2);

	QHBoxLayout * positionLayout = new QHBoxLayout(mainLayout);

	positionLayout->addWidget(seekSlider_);
  positionLayout->addSpacing(2);
	positionLayout->addWidget(positionLabel_);

  mainLayout->addSpacing(2);

	QHBoxLayout * buttonLayout	= new QHBoxLayout(mainLayout);

	buttonLayout->addWidget(playListToggleButton_);
	buttonLayout->addWidget(previousButton_);
	buttonLayout->addWidget(nextButton_);
	buttonLayout->addWidget(stopButton_);
	buttonLayout->addWidget(restartButton_);
	buttonLayout->addWidget(playButton_);

	playListToggleButton_	->installEventFilter(this);
	previousButton_				->installEventFilter(this);
	nextButton_						->installEventFilter(this);
	stopButton_						->installEventFilter(this);
	restartButton_				->installEventFilter(this);
	playButton_						->installEventFilter(this);
	seekSlider_						->installEventFilter(this);
	positionLabel_				->installEventFilter(this);
	titleLabel_						->installEventFilter(this);

	connect
		(
		 playListToggleButton_,
		 SIGNAL(clicked()),
		 napp->player(),
		 SLOT(toggleListView())
		);

	connect
		(
		 previousButton_,
		 SIGNAL(clicked()),
		 napp->player(),
		 SLOT(back())
		);

	connect
		(
		 nextButton_,
		 SIGNAL(clicked()),
		 napp->player(),
		 SLOT(forward())
		);

	connect
		(
		 stopButton_,
		 SIGNAL(clicked()),
		 napp->player(),
		 SLOT(stop())
		);

	connect(restartButton_, SIGNAL(clicked()), SLOT(slotRestart()));

	connect
		(
		 playButton_,
		 SIGNAL(clicked()),
		 napp->player(),
		 SLOT(playpause())
		);

	connect(napp,						SIGNAL(hideYourself()), 	SLOT(hide()));
	connect(napp,						SIGNAL(showYourself()), 	SLOT(show()));

	connect(napp->player(), SIGNAL(playlistShown()),	SLOT(slotPlayListShown()));
	connect(napp->player(), SIGNAL(playlistHidden()),	SLOT(slotPlayListHidden()));
	connect(napp->player(), SIGNAL(playing()),				SLOT(slotPlaying()));
	connect(napp->player(), SIGNAL(stopped()),				SLOT(slotStopped()));
	connect(napp->player(), SIGNAL(paused()),					SLOT(slotPaused()));

	connect(napp->player(), SIGNAL(timeout()),				SLOT(slotTimeout()));

	connect
		(
		 napp->player(),
		 SIGNAL(volumeChanged(int)),
		 SLOT(slotVolumeChanged(int))
		);

	connect
		(
		 new CharlatanConfigModule(this),
		 SIGNAL(saved()),
		 SLOT(slotConfigChanged())
		);

	setCaption(i18n("Noatun"));
	setIcon(BarIcon("noatun"));

	napp->player()->handleButtons();

	loadConfig();

	// Using QAccel, because I don't want any strings associated or
	// configurability.

	QAccel * a = new QAccel(this, "Accelerators");

	int accelPlay = a->insertItem(Key_Space);
	int accelNext = a->insertItem(Key_H);
	int accelPrev = a->insertItem(Key_L);
	int accelStop = a->insertItem(Key_Escape);
	int accelRstt = a->insertItem(Key_Return);
	int accelQuit = a->insertItem(Key_Q);

	a->connectItem(accelPlay, napp->player(), SLOT(playpause()));
	a->connectItem(accelNext, napp->player(), SLOT(forward()));
	a->connectItem(accelPrev, napp->player(), SLOT(back()));
	a->connectItem(accelStop, napp->player(), SLOT(stop()));
	a->connectItem(accelRstt, this, SLOT(slotRestart()));
	a->connectItem(accelQuit, napp, SLOT(quit()));

	show();

	setTitleText();
}

Charlatan::~Charlatan()
{
	// Empty.
}

void Charlatan::closeEvent(QCloseEvent *)
{
	unload();
}

void Charlatan::dragEnterEvent(QDragEnterEvent *event)
{
	event->accept(KURLDrag::canDecode(event));
}

void Charlatan::dropEvent(QDropEvent *event)
{
	KURL::List uri;
	if (KURLDrag::decode(event, uri))
	{
		for (KURL::List::Iterator i = uri.begin(); i != uri.end(); ++i)
			napp->player()->openFile(*i, false);
	}

}

void Charlatan::setTitleText(const QString & s)
{
	QString titleText;

	if (!s.isNull())
	{
		titleText = s;
	}
	else
	{
		Player * player = napp->player();

		if (0 == player)
		{
			qWarning("Noatun's player is null. WTF ?");
			return;
		}
		else
		{
			PlaylistItem currentItem = player->current();

			if (!!currentItem)
			{
				titleText = currentItem.title();
			}
		}
	}

	if (title_ != titleText)
	{
		showingVolumeTimer_->stop();
		title_ = titleText;

		if (!showingVolumeTimer_->isActive())
			titleLabel_->setText(title_);
	}
}

void Charlatan::slotPlaying()
{
	setTitleText();

	playButton_->setOn(true);
	stopButton_->setEnabled(true);
	playButton_->setPixmap(QPixmap((const char **)px_pause));
}

void Charlatan::slotStopped()
{
	setTitleText(i18n("No File Loaded"));

	stopButton_->setEnabled(false);
	playButton_->setOn(false);
	playButton_->setPixmap(QPixmap((const char **)px_play1));
}

void Charlatan::slotPaused()
{
	setTitleText();

	stopButton_->setEnabled(true);
	playButton_->setOn(false);
	playButton_->setPixmap(QPixmap((const char **)px_play1));
}

bool Charlatan::eventFilter(QObject *o, QEvent *e)
{
	switch (e->type())
	{
		case QEvent::Wheel:
			wheelEvent(static_cast<QWheelEvent*>(e));
			return true;
			break;

		default:
			break;
	}

  return QWidget::eventFilter(o, e);
}

void Charlatan::slotPlayListShown()
{
	playListToggleButton_->setOn(true);
}

void Charlatan::slotPlayListHidden()
{
	playListToggleButton_->setOn(false);
}

void Charlatan::mousePressEvent(QMouseEvent * e)
{
	if (e->button() == RightButton)
	{
		NoatunStdAction::ContextMenu::showContextMenu();
		return;
	}

	return QWidget::mousePressEvent(e);
}

void Charlatan::wheelEvent(QWheelEvent * e)
{
	int newVolume = napp->player()->volume() + (e->delta() / 120) * 4;
	napp->player()->setVolume(newVolume);
}

void Charlatan::slotConfigChanged()
{
	loadConfig();
}

void Charlatan::slotVolumeChanged(int i)
{
	QString message(i18n("Volume: %1").arg(i));

	showingVolumeTimer_->start(2000, true);
	titleLabel_->setText(message);
}

void Charlatan::slotStopShowingVolume()
{
	titleLabel_->setText(title_);
}

void Charlatan::slotTimeout()
{
	// noatun bug: napp->player()->lengthString() will crash if there's
	// no 'current track'.
	if (!napp->player()->current())
		return;

	setTitleText();

	QString lengthText(napp->player()->lengthString());

	if (positionLabel_->text() != lengthText)
		positionLabel_->setText(lengthText);
}

void Charlatan::loadConfig()
{
	KConfig * c(KGlobal::config());
	c->setGroup("CharlatanInterface");
	titleLabel_->setScroll(c->readBoolEntry("ScrollTitle", true));
}

void Charlatan::slotRestart()
{
	napp->player()->skipTo(0);
}

#include "userinterface.moc"
// vim:ts=2:sw=2:tw=78:noet
