/*****************************************************************

Copyright (c) 2000-2001 the noatun authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIAB\ILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kvideowidget.h>
#include <qdragobject.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qwidget.h>
#include <qvbox.h>

#include "view.h"
#include "player.h"

namespace
{
QButton *createButton(const QIconSet &_iconset, const QString &_tip, QObject *_receiver, const char *_slot, QWidget *_parent)
{
	QToolButton *button = new QToolButton(_parent);
	button->setMaximumSize(50, 50);
	button->setIconSet(_iconset);
	QToolTip::add(button, _tip);
	QObject::connect(button, SIGNAL(clicked()), _receiver, _slot);
	button->show();
	return button;
}
}

Kaboodle::View::View(QWidget *parent, const char *name, Player *p)
	: KMediaPlayer::View(parent, name)
	, state((KMediaPlayer::Player::State)p->state())
	, autoPlay(false)
	, quitAfterPlaying(false)
	, player(p)
	, firstVideo(false)
	, lastWidth(0)
{
	(new QHBoxLayout(this))->setAutoAdd(true);
	QVBox *box = new QVBox(this);
	box->setSpacing(KDialog::spacingHint());
	box->setMargin(0);

	video = new KVideoWidget(player, box);
	video->actionCollection()->readShortcutSettings();
	setVideoWidget(video);
	connect(video, SIGNAL(adaptSize(int, int)), this, SLOT(calculateSize(int, int)));
	connect(video, SIGNAL(mouseButtonPressed(int, const QPoint&, int)), this, SLOT(slotButtonPressed(int, const QPoint &, int) ) ) ;
	connect(video, SIGNAL(mouseButtonDoubleClick(const QPoint&, int)), this, SLOT(slotDblClick(const QPoint &, int) ) ) ;

	QWidget *sliderBox = new QWidget(box);
	sliderBox->setFocusPolicy(QWidget::ClickFocus);
	sliderBox->setAcceptDrops(true);

	QHBoxLayout *layout = new QHBoxLayout(sliderBox);
	layout->setSpacing(KDialog::spacingHint());
	layout->setMargin(0);
	layout->setAutoAdd(true);

	playButton = createButton(BarIconSet("1rightarrow"), i18n("Play"), player, SLOT(play()), sliderBox);
	pauseButton = createButton(BarIconSet("player_pause"), i18n("Pause"), player, SLOT(pause()), sliderBox);
	stopButton = createButton(BarIconSet("player_stop"), i18n("Stop"), player, SLOT(stop()), sliderBox);

	slider = new L33tSlider(0, 1000, 10, 0, L33tSlider::Horizontal, sliderBox);
	slider->setTickmarks(QSlider::NoMarks);
	slider->show();

	elapsedLabel = new QLabel(sliderBox);
	QFont labelFont = elapsedLabel->font();
	labelFont.setPointSize(24);
	labelFont.setBold(true);
	QFontMetrics labelFontMetrics(labelFont);
	elapsedLabel->setFont(labelFont);
	elapsedLabel->setAlignment(AlignCenter | AlignVCenter | ExpandTabs);
	elapsedLabel->setFixedHeight(labelFontMetrics.height());
	elapsedLabel->setMinimumWidth(labelFontMetrics.width("00:00"));

	connect(player, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
	connect(player, SIGNAL(completed()), this, SLOT(playerFinished()));
	connect(player, SIGNAL(timeout()), this, SLOT(playerTimeout()));

	connect(slider, SIGNAL(userChanged(int)), this, SLOT(skipToWrapper(int)));
	connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
	slider->setEnabled(false);

	connect(this, SIGNAL(buttonsChanged(int)), this, SLOT(updateButtons(int)));
	updateButtons(buttons());

	updateLabel("--:--/--:--");

	video->setMinimumHeight(0);
}

Kaboodle::View::~View(void)
{
	embed(Arts::PlayObject::null());
}

void Kaboodle::View::stateChanged(int s)
{
	KMediaPlayer::Player::State oldState = state;
	state = (KMediaPlayer::Player::State)s;

	switch(state)
	{
	case KMediaPlayer::Player::Play:
		stopButton->setEnabled(true);
		playButton->setEnabled(false);
		pauseButton->setEnabled(true);
		break;
	
	case KMediaPlayer::Player::Empty:
		slider->setEnabled(false);
		slider->setValue(0);
		updateLabel("--:--");
		stopButton->setEnabled(false);
		playButton->setEnabled(false);
		pauseButton->setEnabled(false);
		break;

	case KMediaPlayer::Player::Stop:
		slider->setEnabled(false);
		slider->setValue(0);
		updateLabel("00:00");
		stopButton->setEnabled(false);
		playButton->setEnabled(true);
		pauseButton->setEnabled(false);

		// loaded
		if(oldState == KMediaPlayer::Player::Empty)
		{
			firstVideo = true;
			if(autoPlay) player->play();
		}

		break;
	
	case KMediaPlayer::Player::Pause:
		slider->setEnabled(player->isSeekable());
		stopButton->setEnabled(true);
		playButton->setEnabled(true);
		pauseButton->setEnabled(false);
		break;
	}
}

void Kaboodle::View::playerFinished()
{
	if(quitAfterPlaying) kapp->quit();
}

void Kaboodle::View::playerTimeout()
{
	if(player->currentURL().isEmpty())
		return;

	if(slider->currentlyPressed())
		return;

	updateTicks();

	if(firstVideo)
	{
		if(!lastWidth)
		{
			video->setNormalSize();
		}
		else
		{
			firstVideo = false;
			lastWidth = 0;
		}
	}

	if(player->isSeekable())
	{
		slider->setEnabled(true);
		slider->setValue((int)(player->position() / 1000));
	}

	updateLabel( player->positionString() );
}

void Kaboodle::View::updateTicks(void)
{
	if(player->hasLength())
	{
		int range = (int)(player->length() / 1000);
		slider->setRange(0, range);
	}
	else
	{
		slider->setRange(0, 1);
	}
}

void Kaboodle::View::sliderMoved(int seconds)
{
	if(!player->currentURL().isEmpty())
	updateLabel(Player::timeString(seconds*1000));
}

void Kaboodle::View::skipToWrapper(int second)
{
	player->seek((unsigned long)(second*1000));
}

void Kaboodle::View::updateLabel(const QString &text)
{
	if(elapsedLabel)
		elapsedLabel->setText(text.left(5));
}

void Kaboodle::View::calculateSize(int newWidth, int newHeight)
{
	lastWidth = newWidth;
	int extraWidth = width() - video->width();
	int extraHeight = height() - video->height();
	newWidth += extraWidth;
	newHeight += extraHeight;
	emit adaptSize(newWidth, newHeight);
}

bool Kaboodle::View::isAutoPlay()
{
	return autoPlay;
}

void Kaboodle::View::setAutoPlay(bool b)
{
	autoPlay = b;
}

bool Kaboodle::View::isQuitAfterPlaying()
{
	return quitAfterPlaying;
}

void Kaboodle::View::setQuitAfterPlaying(bool b)
{
	quitAfterPlaying = b;
}

void Kaboodle::View::embed(Arts::PlayObject object)
{
	video->embed(Arts::DynamicCast(object));
}

void Kaboodle::View::updateButtons(int b)
{
	if(b & Play)
		playButton->show();
	else
		playButton->hide();

	if(b & Pause)
		pauseButton->show();
	else
		pauseButton->hide();

	if(b & Stop)
		stopButton->show();
	else
		stopButton->hide();

	if(b & Seeker)
	{
		slider->show();
		elapsedLabel->show();
	}
	else
	{
		slider->hide();
		elapsedLabel->hide();
	}

}

void Kaboodle::View::slotButtonPressed(int /*type*/, const QPoint &, int /* state */)
{
	if((KMediaPlayer::Player::State)player->state() == KMediaPlayer::Player::Pause )
		player->play();
	else player->pause();
}

void Kaboodle::View::slotDblClick( const QPoint &, int /* state */)
{
	if ( video->isFullscreen() )
		video->setWindowed();
	else  video->setFullscreen(); 
	
	player->play(); // play() is called because the video is stopped when double-clicking ( slotButtonPressed is called )
}

#include "view.moc"
