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

#include "seeker.h"
#include "userinterface.h"
#include <noatun/app.h>
#include <noatun/player.h>
#include <noatun/controls.h>
#include <noatun/effects.h>
#include <noatun/engine.h>

#include <qtoolbutton.h>
#include <qdragobject.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qframe.h>

#include <kstatusbar.h>
#include <kpopupmenu.h>
#include <kurldrag.h>
#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kurldrag.h>
#include <noatun/playlist.h>
#include <noatun/stdaction.h>

#include <arts/artsmodules.h>
#include <arts/reference.h>
#include <arts/soundserver.h>
#include <arts/kmedia2.h>

#include "configmodule.h"

AlsaPlayer::AlsaPlayer()
	: APMainWindow	(0, "AlsaPlayer"),
		UserInterface	(),
		speed_				(100)
{
	connect
		(
		 playlistButton,
		 SIGNAL(clicked()),
		 napp->player(),
		 SLOT(toggleListView())
		);

	connect
		(
		 previousButton,
		 SIGNAL(clicked()),
		 napp->player(),
		 SLOT(back())
		);

	connect
		(
		 nextButton,
		 SIGNAL(clicked()),
		 napp->player(),
		 SLOT(forward())
		);

	connect
		(
		 stopButton,
		 SIGNAL(clicked()),
		 napp->player(),
		 SLOT(stop())
		);

	connect
		(
		 playButton,
		 SIGNAL(clicked()),
		 napp->player(),
		 SLOT(playpause())
		);

	connect(napp, SIGNAL(hideYourself()), 	SLOT(hide()));
	connect(napp, SIGNAL(showYourself()), 	SLOT(show()));

	connect(napp->player(), SIGNAL(playlistShown()),	SLOT(slotPlayListShown()));
	connect(napp->player(), SIGNAL(playlistHidden()),	SLOT(slotPlayListHidden()));
	connect(napp->player(), SIGNAL(playing()),				SLOT(slotPlaying()));
	connect(napp->player(), SIGNAL(stopped()),				SLOT(slotStopped()));
	connect(napp->player(), SIGNAL(paused()),					SLOT(slotPaused()));

	connect(napp->player(), SIGNAL(timeout()),				SLOT(slotTimeout()));
	connect(napp->player(), SIGNAL(changed()),				SLOT(slotTrackChanged()));

	connect
		(
		 napp->player(),
		 SIGNAL(volumeChanged(int)),
		 SLOT(slotVolumeChanged(int))
		);

	connect
		(
		 new AlsaPlayerConfigModule(this),
		 SIGNAL(saved()),
		 SLOT(slotConfigChanged())
		);

	connect
		(
		 volumeSlider,
		 SIGNAL(valueChanged(int)),
		 napp->player(),
		 SLOT(setVolume(int))
		);

	connect
		(
		 forwardButton,
		 SIGNAL(clicked()),
		 SLOT(slotForward())
		);

	connect
		(
		 pauseButton,
		 SIGNAL(clicked()),
		 SLOT(slotPause())
		);

	connect(speedSlider, SIGNAL(valueChanged(int)), SLOT(slotSetSpeed(int)));

	menuButton->setPopup(NoatunStdAction::ContextMenu::contextMenu());

	setIcon(BarIcon("noatun"));

	napp->player()->handleButtons();

	loadConfig();

	resize(sizeHint().width(), minimumSizeHint().height());

	show();
}

AlsaPlayer::~AlsaPlayer()
{
	// Empty.
}

void AlsaPlayer::closeEvent(QCloseEvent *)
{
	unload();
}

void AlsaPlayer::dragEnterEvent(QDragEnterEvent *event)
{
	event->accept(KURLDrag::canDecode(event));
}

void AlsaPlayer::dropEvent(QDropEvent *event)
{
	KURL::List uri;
	if (KURLDrag::decode(event, uri))
	{
		for (KURL::List::Iterator i = uri.begin(); i !=	uri.end(); ++i)
			napp->player()->openFile(*i, false);
	}
}

void AlsaPlayer::setTitleText(const QString & s)
{
	QString titleText
		(s.isNull() ? napp->player()->current().title() : s);

	if (titleLabel->text() != titleText)
		titleLabel->setText(titleText);
}

void AlsaPlayer::slotPlaying()
{
	setTitleText(QString::null);

	playButton->setOn(true);
	stopButton->setEnabled(true);
}

void AlsaPlayer::slotStopped()
{
	setTitleText(i18n("No File Loaded"));

	stopButton->setEnabled(false);
	playButton->setOn(false);
}

void AlsaPlayer::slotPaused()
{
	setTitleText(QString::null);

	stopButton->setEnabled(true);
	playButton->setOn(false);
}

bool AlsaPlayer::eventFilter(QObject *o, QEvent *e)
{
	switch (e->type())
	{
		case QEvent::MouseButtonPress:
			mousePressEvent(static_cast<QMouseEvent *>(e));
			break;

		case QEvent::Wheel:
			wheelEvent(static_cast<QWheelEvent*>(e));
			return true;
			break;

		default:
			break;
	}

  return QWidget::eventFilter(o, e);
}

void AlsaPlayer::slotPlayListShown()
{
	playlistButton->setOn(true);
}

void AlsaPlayer::slotPlayListHidden()
{
	playlistButton->setOn(false);
}

void AlsaPlayer::mousePressEvent(QMouseEvent * e)
{
	if (e->button() == RightButton)
	{
		NoatunStdAction::ContextMenu::showContextMenu();
		return;
	}

	return QWidget::mousePressEvent(e);
}

void AlsaPlayer::wheelEvent(QWheelEvent * e)
{
	int newVolume = napp->player()->volume() + (e->delta() / 120);
	napp->player()->setVolume(newVolume);
}

void AlsaPlayer::slotConfigChanged()
{
	loadConfig();
}

void AlsaPlayer::slotVolumeChanged(int i)
{
	QString text("%1%");
	volumeLabel->setText(text.arg(i));
	volumeSlider->setValue(i);
}

void AlsaPlayer::slotTimeout()
{
	// noatun bug: napp->player()->lengthString() will crash if there's
	// no 'current track'.
	if (!napp->player()->current())
		return;

	setTitleText(QString::null);

	QString lengthText(napp->player()->lengthString());

	if (timeLabel->text() != lengthText)
		timeLabel->setText(lengthText);
}

void AlsaPlayer::loadConfig()
{
	// Empty.
}

void AlsaPlayer::slotRestart()
{
	napp->player()->skipTo(0);
}

void AlsaPlayer::slotTrackChanged()
{
	slotSetSpeed(speed_);
}

void AlsaPlayer::slotSetSpeed(int newSpeed)
{
	speed_ = newSpeed;

	pauseButton->setEnabled(0 != speed_);
	speedLabel->setText(QString("%1%").arg(speed_));

	Arts::PlayObject playobject(napp->player()->engine()->playObject());
	Arts::PitchablePlayObject pitchable = Arts::DynamicCast(playobject);

	if (!pitchable.isNull())
	{
		float f = (float(speed_) / 100.f);
		pitchable.speed(f);
	}
}

void AlsaPlayer::slotPause()
{
	speedSlider->setValue(0);
	pauseButton->setEnabled(false);
}

void AlsaPlayer::slotForward()
{
	speedSlider->setValue(100);
}

#include "userinterface.moc"
// vim:ts=2:sw=2:tw=78:noet
