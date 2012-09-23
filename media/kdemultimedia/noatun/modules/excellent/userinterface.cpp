// userinterface.cpp
//
// Copyright (C) 2001 Neil Stevens <neil@qualityassistant.com>
// Copyright (C) 1999 Charles Samuels <charles@kde.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// THE AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name(s) of the author(s) shall not be
// used in advertising or otherwise to promote the sale, use or other dealings
// in this Software without prior written authorization from the author(s).

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <noatun/effects.h>
#include <noatun/app.h>
#include <noatun/controls.h>
#include <noatun/pref.h>
#include <noatun/player.h>

#include "userinterface.h"

#include <kbuttonbox.h>
#include <kconfig.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenubar.h>
#include <kmessagebox.h>
#include <kpixmapeffect.h>
#include <kpopupmenu.h>
#include <kstatusbar.h>
#include <kstdaction.h>
#include <kwin.h>
#include <kurldrag.h>

#include <qbitmap.h>
#include <qdragobject.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qobjectlist.h>
#include <qobjectdict.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qvbox.h>

Excellent::Excellent()
	: KMainWindow(0, "NoatunExcellent")
	, UserInterface()
{
	setAcceptDrops(true);

	KStdAction::quit(napp, SLOT(quit()), actionCollection());
	KStdAction::open(napp, SLOT(fileOpen()), actionCollection());
	setStandardToolBarMenuEnabled(true);

	menubarAction = KStdAction::showMenubar(this, SLOT(showMenubar()), actionCollection());
	KStdAction::preferences(napp, SLOT(preferences()), actionCollection());

	// buttons
	NoatunStdAction::back(actionCollection(), "back");
	NoatunStdAction::stop(actionCollection(), "stop");
	NoatunStdAction::play(actionCollection(), "play");
	NoatunStdAction::pause(actionCollection(), "pause");
	NoatunStdAction::forward(actionCollection(), "forward");
	NoatunStdAction::playlist(actionCollection(), "show_playlist");

	volumeAction = new KToggleAction(i18n("Show &Volume Control"), 0, this, SLOT(showVolumeControl()), actionCollection(), "show_volumecontrol");
	volumeAction->setCheckedState(i18n("Hide &Volume Control"));
	NoatunStdAction::effects(actionCollection(), "effects");
	NoatunStdAction::equalizer(actionCollection(), "equalizer");

	NoatunStdAction::loop(actionCollection(), "loop_style");

	createGUI("excellentui.rc");

	napp->pluginActionMenu()->plug(menuBar(),3);
	toolBar("mainToolBar")->hide();

	// Who needs Qt Designer?
	mainFrame = new QHBox(this);
	mainFrame->setSpacing(KDialog::spacingHint());
	mainFrame->setMargin(0);
	slider = new L33tSlider(0, 1000, 10, 0, L33tSlider::Horizontal, mainFrame);
	slider->setTickmarks(QSlider::NoMarks);

	elapsed = new QLabel(mainFrame);
	QFont labelFont = elapsed->font();
	labelFont.setPointSize(24);
	labelFont.setBold(true);
	QFontMetrics labelFontMetrics = labelFont;
	elapsed->setFont(labelFont);
	elapsed->setAlignment(AlignCenter | AlignVCenter | ExpandTabs);
	elapsed->setText("--:--");
	elapsed->setFixedHeight(labelFontMetrics.height());
	elapsed->setMinimumWidth(elapsed->sizeHint().width());

	// Doing this makes the slider the same heigh as the font. This is just plain wrong...
	//slider->setFixedHeight(labelFontMetrics.height());

	setCentralWidget(mainFrame);

	total = new QLabel(statusBar());
	labelFont = total->font();
	labelFont.setBold(true);
	total->setFont(labelFont);
	total->setAlignment(AlignCenter | AlignVCenter | ExpandTabs);
	total->setText("--:--");
	total->setMinimumWidth(total->sizeHint().width());
	total->setText("");

	statusBar()->addWidget(total, 0, true);
	statusBar()->show();

	connect( napp, SIGNAL(hideYourself()), this, SLOT(hide()) );
	connect( napp, SIGNAL(showYourself()), this, SLOT(show()) );

	connect(napp->player(), SIGNAL(playing()), this, SLOT(slotPlaying()));
	connect(napp->player(), SIGNAL(stopped()), this, SLOT(slotStopped()));
	connect(napp->player(), SIGNAL(paused()), this, SLOT(slotPaused()));
	napp->player()->handleButtons();

	connect(napp->player(), SIGNAL(timeout()), this, SLOT(slotTimeout()));
	connect(napp->player(), SIGNAL(loopTypeChange(int)), this, SLOT(slotLoopTypeChanged(int)));

	/* This skipToWrapper is needed to pass milliseconds to Player() as everybody
	 * below the GUI is based on milliseconds instead of some unprecise thingy
	 * like seconds or mille */
	connect(slider, SIGNAL(userChanged(int)), this, SLOT(skipToWrapper(int)));
	connect(this, SIGNAL(skipTo(int)), napp->player(), SLOT(skipTo(int)));

	connect(slider, SIGNAL(sliderMoved(int)), SLOT(sliderMoved(int)));

	setCaption("Noatun");
	setIcon(SmallIcon("noatun"));
	//slotLoopTypeChanged(Player::None);
	changeStatusbar();
	handleLengthString("--:--/--:--");

	setMinimumWidth(250);
	resize(300, 75);

	KConfig &config = *KGlobal::config();

	toolBar("main")->applySettings(&config, "Excellent main");

	config.setGroup("excellent");

	volumeSlider = 0;
	volumeAction->setChecked( config.readBoolEntry("volumeShown", false) );
	showVolumeControl();

	menubarAction->setChecked( config.readBoolEntry("menuShown", true) );
	showMenubar();

	applyMainWindowSettings(&config, "excellent");

	switch((NET::MappingState)config.readNumEntry("mappingState", (int)NET::Visible))
	{
	case NET::Visible:
		showNormal();
		break;
	case NET::Withdrawn:
		if (napp->libraryLoader()->isLoaded("systray.plugin"))
			hide();
		break;
	case NET::Iconic:
		showMinimized();
		break;
	}

	for (QPtrListIterator<QObject> i(*children()); i.current(); ++i)
		(*i)->installEventFilter(this);
}

Excellent::~Excellent()
{
	KConfig &config = *KGlobal::config();
	saveMainWindowSettings(&config, "excellent");
	toolBar("main")->saveSettings(&config, "Excellent main");
	config.setGroup("excellent");
	config.writeEntry("volumeShown", volumeAction->isChecked());
	config.writeEntry("menuShown", menubarAction->isChecked());
	config.writeEntry("width", width());
	config.writeEntry("height", height());
	config.sync();
}

void Excellent::showEvent(QShowEvent *e)
{
	KConfig *config = KGlobal::config();
	config->setGroup("excellent");
	config->writeEntry("mappingState", NET::Visible);
	config->sync();

	KMainWindow::showEvent(e);
}

void Excellent::hideEvent(QHideEvent *e)
{
	KConfig *config = KGlobal::config();
	config->setGroup("excellent");
	config->writeEntry("mappingState", NET::Withdrawn);
	config->sync();

	KMainWindow::hideEvent(e);
}

void Excellent::closeEvent(QCloseEvent *)
{
	unload();
}

void Excellent::dragEnterEvent(QDragEnterEvent *event)
{
	// accept uri drops only
	event->accept(KURLDrag::canDecode(event));
}

void Excellent::dropEvent(QDropEvent *event)
{
	KURL::List uri;
	if (KURLDrag::decode(event, uri))
	{
		for (KURL::List::Iterator i = uri.begin(); i != uri.end(); ++i)
			napp->player()->openFile(*i, false);
	}
}

bool Excellent::eventFilter(QObject *o, QEvent *e)
{
	if (e->type() == QEvent::Wheel)
	{
		wheelEvent(static_cast<QWheelEvent*>(e));
		return true;
	}
	return QWidget::eventFilter(o, e);
}

void Excellent::wheelEvent(QWheelEvent *e)
{
	int delta=e->delta();
	napp->player()->setVolume(napp->player()->volume() + delta/120*2);
}

void Excellent::slotPlaying()
{
	slider->setEnabled(true);
	changeStatusbar();
}

void Excellent::slotStopped()
{
	slider->setEnabled(false);
	if(!napp->player()->current()) return;
	changeStatusbar();
	slider->setValue(0);
	handleLengthString("--:--/--:--");
}

void Excellent::slotPaused()
{
	slider->setEnabled(true);
	changeStatusbar();
}

void Excellent::slotTimeout()
{
	if(volumeSlider) volumeSlider->setValue(100 - napp->player()->volume());

	if (!slider->currentlyPressed())
		handleLengthString(napp->player()->lengthString());

	if(!napp->player()->current()) return;
	if(slider->currentlyPressed()) return;

	slider->setRange(0, (int)napp->player()->getLength() / 1000 );
	slider->setValue((int)napp->player()->getTime() / 1000 );

	changeStatusbar();
}

void Excellent::sliderMoved(int seconds)
{
	if(napp->player()->current())
		handleLengthString(napp->player()->lengthString(seconds * 1000));
}

void Excellent::skipToWrapper(int second) // wrap int to int _and_ seconds to mseconds
{
	emit skipTo((int)(second*1000));
}

void Excellent::slotLoopTypeChanged(int type)
{
	static const int time = 2000;
	switch (type)
	{
	case Player::None:
		statusBar()->message(i18n("No looping"), time);
		break;
	case Player::Song:
		statusBar()->message(i18n("Song looping"), time);
		break;
	case Player::Playlist:
		statusBar()->message(i18n("Playlist looping"), time);
		break;
	case Player::Random:
		statusBar()->message(i18n("Random play"), time);
	}
}

void Excellent::showMenubar(void)
{
	if(menubarAction->isChecked())
	{
		menuBar()->show();
	}
	else
	{
		KMessageBox::information(this, i18n("<qt>Press %1 to show the menubar.</qt>").arg(menubarAction->shortcut().toString()), QString::null, "Hide Menu warning");
		menuBar()->hide();
	}
}

void Excellent::showVolumeControl(void)
{
	if(volumeAction->isChecked())
		growVolumeControl();
	else
		shrinkVolumeControl();
}

void Excellent::changeStatusbar(void)
{
	if(!napp->player()->current().isNull())
		statusBar()->message(napp->player()->current().title());
}

void Excellent::handleLengthString(const QString &text)
{
	if(text.right(5) == "00:00" && text.left(5) == "00:00")
	{
		elapsed->setText("--:--");
		total->setText("--:--");
	}
	else
	{
		// Split the length string in to "current" and "elapsed"
		QStringList tokens = QStringList::split("/", text);

		elapsed->setText(tokens[0]);
		total->setText(tokens[1]);
	}
}

void Excellent::growVolumeControl(void)
{
	volumeSlider = new L33tSlider(0, 100, 10, 0, Vertical, mainFrame);
	volumeSlider->setValue(100 - napp->player()->volume());
	volumeSlider->show();
	connect(volumeSlider, SIGNAL(sliderMoved(int)), SLOT(changeVolume(int)));
	connect(volumeSlider, SIGNAL(userChanged(int)), SLOT(changeVolume(int)));
}

void Excellent::shrinkVolumeControl(void)
{
	delete volumeSlider;
	volumeSlider = 0;
}

void Excellent::changeVolume(int slider)
{
	napp->player()->setVolume(100 - slider);
}

#include "userinterface.moc"
