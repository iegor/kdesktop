/*
  Winamp Skin
  Copyright (C) 1999  Martin Vogt
  Copyright (C) 2001-2002  Ryan Cumming

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include <kconfig.h>
#include <qstringlist.h>
#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>
#include <kurldrag.h>
#include <kstdaction.h>
#include <kaction.h>
#include <qdragobject.h>
#include <kstandarddirs.h>
#include <kwin.h>
#include <time.h>

#include <noatun/playlist.h>
#include <noatun/app.h>
#include <noatun/stdaction.h>

#include "waSkin.h"

#include "waRegion.h"

#include "waMain.h"
#include "waClutterbar.h"
#include "waDigit.h"
#include "waInfo.h"
#include "waButton.h"
#include "waLabel.h"
#include "waIndicator.h"
#include "waTitleBar.h"

#include "waJumpSlider.h"
#include "waBalanceSlider.h"
#include "waVolumeSlider.h"

#include "waStatus.h"

#include "waSkinManager.h"
#include "waSkinModel.h"

#include "guiSpectrumAnalyser.h"
#include "fileInfo.h"

WaSkin *_waskin_instance = NULL;

WaSkin::WaSkin() : QWidget(0, "NoatunWinampSkin"), UserInterface()
{
    mJumpPressed = false;
    mVolumePressed = false;
    mBalancePressed = false;

    _waskin_instance = this;

    // Our really low-level render-y sort of stuff
    waSkinModel = new WaSkinModel();
    // Skin management stuff
    waSkinManager = new WaSkinManager();

    createHighLevelElements();
    createButtons();
    setMinimumSize(sizeHint());
    setMaximumSize(sizeHint());

    KWin::setType(this->winId(), NET::Override);
    setBackgroundMode(NoBackground);

    setAcceptDrops(true);

    title_shaded = false;

    // These slots make Young Hickory love us
    connect( napp, SIGNAL(hideYourself()), this, SLOT(hide()) );
    connect( napp, SIGNAL(showYourself()), this, SLOT(show()) );

    connect(napp->player(), SIGNAL(playlistShown()), this,
            SLOT(playlistShown()));
    connect(napp->player(), SIGNAL(playlistHidden()), this,
            SLOT(playlistHidden()));
    connect(napp->player(), SIGNAL(loopTypeChange(int)), this,
            SLOT(loopChange(int)));
    connect(napp->player(), SIGNAL(newSong()), this, SLOT(newSong()));

    connect(napp->player(), SIGNAL(timeout()), this, SLOT(timetick()));
    KConfig *config=KGlobal::config();

    config->setGroup("Winskin");
    QString skin = config->readEntry("CurrentSkin", WaSkin::defaultSkin());

    loadSkin(skin);

    setCaption(i18n("Noatun"));
    setIcon(SmallIcon("noatun"));


    QObject::connect(waTitleBar, SIGNAL(shaded()), this, SLOT(shadeEvent()));
    // connect to players signals - so we can update our display if someone else
    // changes settings...

    connect(napp->player(), SIGNAL(stopped()), this, SLOT(slotStopped()));
    connect(napp->player(), SIGNAL(playing()), this, SLOT(slotPlaying()));
    connect(napp->player(), SIGNAL(paused()), this, SLOT(slotPaused()));

    napp->player()->handleButtons();

    playlist->setToggled(napp->playlist()->listVisible());

    shuffle->setToggled(false);
    repeat->setToggled(false);
    waBalanceSlider->setBalanceValue(0);
    waVolumeSlider->setVolumeValue(napp->player()->volume());

    newSong();

    // HACK: We won't get focus events otherwise
    setFocusPolicy(QWidget::ClickFocus);

    show();
}


WaSkin::~WaSkin()
{
    delete waSkinManager;
    waSkinManager = 0L;
}

void WaSkin::loadSkin(QString newSkinDir)
{
    waSkinManager->loadSkin(newSkinDir);

    setMinimumSize(sizeHint());

    if (title_shaded) {
        // waSkinModel::load() resets our skin model :(
        waSkinModel->setSkinModel(WA_MODEL_WINDOWSHADE);
        setMask(*windowRegion->mainWindowShadeMask());
    }
    else {
        setMask(*windowRegion->mainWindowMask());
    }
}

QSize WaSkin::sizeHint() const
{
    QRect temp_rect;

    temp_rect = waSkinModel->getGeometry(_WA_SKIN_MAIN);

    return temp_rect.size();
}


void WaSkin::createButtons()
{
    prev = new WaButton(_WA_MAPPING_CBUTTONS_PREV);
    play = new WaButton(_WA_MAPPING_CBUTTONS_PLAY);
    pause = new WaButton(_WA_MAPPING_CBUTTONS_PAUSE);
    stop = new WaButton(_WA_MAPPING_CBUTTONS_STOP);
    next = new WaButton(_WA_MAPPING_CBUTTONS_NEXT);
    eject = new WaButton(_WA_MAPPING_CBUTTONS_EJECT);
    shuffle = new WaButton(_WA_MAPPING_SHUFFLE);
    repeat = new WaButton(_WA_MAPPING_REPEAT);
    playlist = new WaButton(_WA_MAPPING_PLAYLIST);
    eq = new WaButton(_WA_MAPPING_EQ);

    menu = new WaButton(_WA_MAPPING_TITLE_MENU);
    menu->setPixmapUp(_WA_SKIN_TITLE_MENU_NORM);
    menu->setPixmapDown(_WA_SKIN_TITLE_MENU_PRES);
    connect(menu, SIGNAL(clicked()), this, SLOT(menuEvent()));

    minimize = new WaButton(_WA_MAPPING_TITLE_MIN);
    minimize->setPixmapUp(_WA_SKIN_TITLE_MIN_NORM);
    minimize->setPixmapDown(_WA_SKIN_TITLE_MIN_PRES);
    connect(minimize, SIGNAL(clicked()), this, SLOT(minimizeEvent()));

    titleshade = new WaButton(_WA_MAPPING_TITLE_SHADE);
    titleshade->setPixmapUp(_WA_SKIN_TITLE_SHADE_NORM);
    titleshade->setPixmapDown(_WA_SKIN_TITLE_SHADE_PRES);
    connect(titleshade, SIGNAL(clicked()), this, SLOT(shadeEvent()));

    close = new WaButton(_WA_MAPPING_TITLE_CLOSE);
    close->setPixmapUp(_WA_SKIN_TITLE_CLOSE_NORM);
    close->setPixmapDown(_WA_SKIN_TITLE_CLOSE_PRES);
    connect(close, SIGNAL(clicked()), this, SLOT(doClose()));

    shuffle->setTogglable(true);
    shuffle->show();

    repeat->setTogglable(true);
    playlist->setTogglable(true);

    connect(shuffle, SIGNAL(toggleEvent(bool)),
	    this, SLOT(shuffleClickedEvent(bool)));

    connect(repeat, SIGNAL(toggleEvent(bool)),
	    this, SLOT(repeatClickedEvent(bool)));

    connect(playlist, SIGNAL(toggleEvent(bool)),
	    this, SLOT(playlistClickedEvent(bool)));

    connect(eq, SIGNAL(clicked()),
	    this, SLOT(eqClickedEvent()));

    prev->setPixmapUp(_WA_SKIN_CBUTTONS_PREV_NORM);
    prev->setPixmapDown(_WA_SKIN_CBUTTONS_PREV_PRES);
    connect(prev, SIGNAL(clicked()), napp->player(), SLOT(back()));

    play->setPixmapUp(_WA_SKIN_CBUTTONS_PLAY_NORM);
    play->setPixmapDown(_WA_SKIN_CBUTTONS_PLAY_PRES);
    connect(play, SIGNAL(clicked()), this, SLOT(playCurrentEvent()));

    pause->setPixmapUp(_WA_SKIN_CBUTTONS_PAUSE_NORM);
    pause->setPixmapDown(_WA_SKIN_CBUTTONS_PAUSE_PRES);
    connect(pause, SIGNAL(clicked()), this, SLOT(playPauseEvent()));


    stop->setPixmapUp(_WA_SKIN_CBUTTONS_STOP_NORM);
    stop->setPixmapDown(_WA_SKIN_CBUTTONS_STOP_PRES);
    connect(stop, SIGNAL(clicked()), napp->player(), SLOT(stop()));


    next->setPixmapUp(_WA_SKIN_CBUTTONS_NEXT_NORM);
    next->setPixmapDown(_WA_SKIN_CBUTTONS_NEXT_PRES);
    connect(next, SIGNAL(clicked()), napp->player(), SLOT(forward()));


    eject->setPixmapUp(_WA_SKIN_CBUTTONS_EJECT_NORM);
    eject->setPixmapDown(_WA_SKIN_CBUTTONS_EJECT_PRESS);
    connect(eject, SIGNAL(clicked()), napp, SLOT(fileOpen()));

    shuffle->setPixmapUp(_WA_SKIN_SHUFREP_SHUFFLE_NOT_SET_NORM);
    shuffle->setPixmapDown(_WA_SKIN_SHUFREP_SHUFFLE_NOT_SET_PRES);
    shuffle->setPixmapUpSelected(_WA_SKIN_SHUFREP_SHUFFLE_SET_NORM);
    shuffle->setPixmapDownSelected(_WA_SKIN_SHUFREP_SHUFFLE_SET_PRES);
    shuffle->setToggled(true);

    repeat->setPixmapUp(_WA_SKIN_SHUFREP_REPEAT_NOT_SET_NORM);
    repeat->setPixmapDown(_WA_SKIN_SHUFREP_REPEAT_NOT_SET_PRES);
    repeat->setPixmapUpSelected(_WA_SKIN_SHUFREP_REPEAT_SET_NORM);
    repeat->setPixmapDownSelected(_WA_SKIN_SHUFREP_REPEAT_SET_PRES);


    eq->setPixmapUp(_WA_SKIN_SHUFREP_EQ_NOT_SET_NORM);
    eq->setPixmapDown(_WA_SKIN_SHUFREP_EQ_NOT_SET_PRES);

    playlist->setPixmapUp(_WA_SKIN_SHUFREP_PLAYLIST_NOT_SET_NORM);
    playlist->setPixmapDown( _WA_SKIN_SHUFREP_PLAYLIST_NOT_SET_PRES);

    playlist->setPixmapUpSelected(_WA_SKIN_SHUFREP_PLAYLIST_SET_NORM);
    playlist->setPixmapDownSelected(_WA_SKIN_SHUFREP_PLAYLIST_SET_PRES);

    waClutterbar = new WaClutterbar();
}

void WaSkin::createHighLevelElements()
{
    // Two most top-level elements
    main = new WaMain();
    waTitleBar = new WaTitleBar();

    guiSpectrumAnalyser = new GuiSpectrumAnalyser();

    waJumpSlider = new WaJumpSlider();
    waJumpSlider->buildGui();

    waVolumeSlider = new WaVolumeSlider();
    waVolumeSlider->buildGui();

    connect(waVolumeSlider, SIGNAL(volumeSetValue(int)),
	    this, SLOT(volumeSetValue(int)));
    connect(waVolumeSlider, SIGNAL(sliderPressed()),
	    this, SLOT(volumeSliderPressed()));
    connect(waVolumeSlider, SIGNAL(sliderReleased()),
	    this, SLOT(volumeSliderReleased()));

    waBalanceSlider = new WaBalanceSlider();
    waBalanceSlider->buildGui();

    connect(waBalanceSlider, SIGNAL(balanceSetValue(int)),
	    this, SLOT(balanceSetValue(int)));
    connect(waBalanceSlider, SIGNAL(sliderPressed()),
            this, SLOT(balanceSliderPressed()));
    connect(waBalanceSlider, SIGNAL(sliderReleased()),
            this, SLOT(balanceSliderReleased()));

    waDigit = new WaDigit();
    connect(waDigit, SIGNAL(digitsClicked()), this, SLOT(digitsClicked()));

    waBPS = new WaLabel(_WA_MAPPING_BPS);
    waFreq = new WaLabel(_WA_MAPPING_FREQ);

    waInfo = new WaInfo();

    waStatus = new WaStatus();

    waStereo = new WaIndicator(_WA_MAPPING_MONOSTER_STEREO, _WA_SKIN_MONOSTER_STEREO_TRUE, _WA_SKIN_MONOSTER_STEREO_FALSE);
    waMono = new WaIndicator(_WA_MAPPING_MONOSTER_MONO, _WA_SKIN_MONOSTER_MONO_TRUE, _WA_SKIN_MONOSTER_MONO_FALSE);

    connect(waJumpSlider, SIGNAL(jump(int)), this, SLOT(jump(int)));
    connect(waJumpSlider, SIGNAL(sliderPressed()),
	    this, SLOT(jumpSliderPressed()));
    connect(waJumpSlider, SIGNAL(sliderReleased()),
	    this, SLOT(jumpSliderReleased()));
    connect(waJumpSlider, SIGNAL(valueChanged(int)),
	    this, SLOT(jumpValueChanged(int)));
}


void WaSkin::setChannels(int val)
{
    if (val <= 0) {
	waStereo->setState(false);
	waMono->setState(false);
    } else if (val == 1) {
	waStereo->setState(false);
	waMono->setState(true);
    } else {
	waStereo->setState(true);
	waMono->setState(false);
    }
}

void WaSkin::shade() {
    waSkinModel->setSkinModel(WA_MODEL_WINDOWSHADE);

    setMinimumSize(sizeHint());
    setMask(*windowRegion->mainWindowShadeMask());

    title_shaded = true;
}

void WaSkin::unshade() {
    waSkinModel->setSkinModel(WA_MODEL_NORMAL);

    setMinimumSize(sizeHint());
    setMask(*windowRegion->mainWindowMask());

    title_shaded = false;
}

void WaSkin::focusOutEvent( QFocusEvent * ) {
    menu->setPixmapUp(_WA_SKIN_TITLE_MENU_INACTIVE);
    menu->update();

    minimize->setPixmapUp(_WA_SKIN_TITLE_MIN_INACTIVE);
    minimize->update();

    titleshade->setPixmapUp(_WA_SKIN_TITLE_SHADE_INACTIVE);
    titleshade->update();

    close->setPixmapUp(_WA_SKIN_TITLE_CLOSE_INACTIVE);
    close->update();

    waTitleBar->setState(false);
}

void WaSkin::focusInEvent( QFocusEvent * ) {
    menu->setPixmapUp(_WA_SKIN_TITLE_MENU_NORM);
    menu->update();

    minimize->setPixmapUp(_WA_SKIN_TITLE_MIN_NORM);
    minimize->update();

    titleshade->setPixmapUp(_WA_SKIN_TITLE_SHADE_NORM);
    titleshade->update();

    close->setPixmapUp(_WA_SKIN_TITLE_CLOSE_NORM);
    close->update();

    waTitleBar->setState(true);
}

void WaSkin::wheelEvent(QWheelEvent *e) {
    // Get the current volume
    int newVolume = napp->player()->volume();

    // Wheel events return needlessly large "deltas", normalize it a bit
    newVolume += e->delta() / 24;
    napp->player()->setVolume(newVolume);
}

void WaSkin::repeatClickedEvent(bool)
{
    updateLoopStyle();
}

void WaSkin::shuffleClickedEvent(bool)
{
    updateLoopStyle();
}

void WaSkin::updateLoopStyle() {
    if (shuffle->toggled()) {
        napp->player()->loop(Player::Random);
    }
    else {
        int loopVal = repeat->toggled() ? Player::Playlist : Player::None;
        napp->player()->loop(loopVal);
    }
}

void WaSkin::playlistClickedEvent(bool)
{
    napp->playlist()->toggleList();
}

void WaSkin::eqClickedEvent()
{
    napp->equalizerView();
}

void WaSkin::jump(int val)
{
    if (napp->player()->isStopped()) {
        waJumpSlider->setJumpValue(0);
    } else {
        napp->player()->skipTo((int) (val * 1000));
    }
}

void WaSkin::jumpSliderPressed()
{
    mJumpPressed = true;
    jumpValueChanged(waJumpSlider->jumpValue());
}


void WaSkin::jumpSliderReleased()
{
    mJumpPressed = false;
    waInfo->setText(getTitleString());
}

void WaSkin::jumpValueChanged(int val)
{
    if (mJumpPressed && !napp->player()->isStopped()) {
        QString timeStr = i18n("Seek to: %1/%2 (%3%)").
		arg(getTimeString(val * 1000)).
		arg(getTimeString(napp->player()->getLength())).
		arg((val * 1000 * 100) / napp->player()->getLength());
        waInfo->setText(timeStr);
    }
}

QString WaSkin::getTitleString() {
    int length;
    QString title = "";

    if (!napp->playlist()->current()) {
        title = "Noatun ";
        title += QString::number(NOATUN_MAJOR) + ".";
        title += QString::number(NOATUN_MINOR) + ".";
        title += QString::number(NOATUN_PATCHLEVEL);
    }
    else {
        length = napp->playlist()->current().length();

        title = napp->playlist()->current().title();

        if (length >= 0)
            title += " (" + getTimeString(length) + ")";

        if (title.length() > 30) {
            // It's scrolling; provide the nice, friendly seperator.
            title += " *** ";
        }
    }

    return title;
}

QString WaSkin::getTimeString(int milliseconds, bool truncate) {
    int seconds = abs(milliseconds / 1000);
    QString ret = "";

    // Do we need to convert to hours:minutes instead of minutes:seconds?
    if (truncate && (abs(seconds) >= (100 * 60)))
        seconds /= 60;

    // Print the optional minus sign, hours/minutes, a colon, and then minutes/seconds.
    ret.sprintf("%s%.2d:%.2d", ((milliseconds < 0) ? "-" : ""), seconds / 60, seconds % 60);

    return ret;
}

void WaSkin::menuEvent() {
    NoatunStdAction::ContextMenu::showContextMenu(mapToGlobal(QPoint(0, 14)));
}

void WaSkin::minimizeEvent() {
    showMinimized();
}

void WaSkin::shadeEvent()
{
    if (!title_shaded)
        shade();
    else
        unshade();
}

void WaSkin::doUnload() {
    unload();
}

void WaSkin::doClose() {
    QTimer::singleShot(0, this, SLOT(doUnload()));
}

void WaSkin::dragEnterEvent(QDragEnterEvent * event)
{
    // accept uri drops only
    event->accept(KURLDrag::canDecode(event));
}

void WaSkin::dropEvent(QDropEvent * event)
{
	KURL::List uri;
	if (KURLDrag::decode(event, uri))
	{
		for (KURL::List::Iterator i = uri.begin(); i != uri.end(); ++i)
			napp->player()->openFile(*i, false);
	}
}

void WaSkin::balanceSliderPressed()
{
    mBalancePressed = true;
    balanceSetValue(0);
}

void WaSkin::balanceSliderReleased()
{

    mBalancePressed = false;
    waBalanceSlider->setBalanceValue(0);

    waInfo->setText(getTitleString());
}

void WaSkin::balanceSetValue(int val)
{
    if (val == 0) {
        waInfo->setText(i18n("Balance: Center"));
    }
    else if (val < 0) {
        waInfo->setText(i18n("Balance: %1% Left").arg(-val));
    } else {
        waInfo->setText(i18n("Balance: %1% Right").arg(val));
   }
}

void WaSkin::playCurrentEvent()
{
    if (napp->player()->isPaused())
        napp->player()->playpause();
    else
        napp->player()->playCurrent();
}

void WaSkin::playPauseEvent()
{
    if (!napp->player()->isStopped())
        napp->player()->playpause();
}


void WaSkin::loopChange(int loopType)
{
    shuffle->setToggled(loopType == Player::Random);

    if (loopType != Player::Random)
        repeat->setToggled(loopType != Player::None);
}

void WaSkin::playlistShown()
{
    playlist->setToggled(true);
}

void WaSkin::playlistHidden()
{
    playlist->setToggled(false);
}

void WaSkin::newSong()
{
    if (napp->player()->getLength() == -1)
        waJumpSlider->hide();
    else
        waJumpSlider->show();

    mJumpPressed = false;
    waJumpSlider->cancelDrag();

    timetick();
}

void WaSkin::timetick()
{
    int mLength;

    if (!mVolumePressed && !mBalancePressed && !mJumpPressed)
        waInfo->setText(getTitleString());

    if (!napp->player()->current())
        return;

    mLength = (int) napp->player()->getLength() / 1000;
    if (mLength < 0)
        mLength = 0;

    waJumpSlider->setJumpRange(mLength);

    digitsClicked();

    int time = 0;
    if (napp->player()->current())
        time = (int) napp->player()->getTime() / 1000;

    if (!mJumpPressed)
        waJumpSlider->setJumpValue(time);

    waVolumeSlider->setVolumeValue(napp->player()->volume());
}

void WaSkin::digitsClicked() {
    if (!waDigit->timeReversed() || (napp->player()->getLength() == -1)) {
        // Setting truncate=true means we want setTime to return
        // no more than 6 digits (-xx:yy)

        if (napp->player()->getTime() != -1)
            waDigit->setTime(getTimeString(napp->player()->getTime(), true));
        else
            waDigit->setTime(getTimeString(0, true));
   }
   else {
        int rem_time = napp->player()->getTime() - napp->player()->getLength();

        // Setting truncate=true means we want setTime to return
        // no more than 6 digits (-xx:yy)
        waDigit->setTime(getTimeString(rem_time, true));
   }
}

void WaSkin::volumeSliderPressed()
{
    mVolumePressed = true;
    volumeSetValue(napp->player()->volume());
}

void WaSkin::volumeSliderReleased()
{
    mVolumePressed = false;
    waInfo->setText(getTitleString());
}

void WaSkin::volumeSetValue(int val)
{
    if (mVolumePressed)
        waInfo->setText(i18n("Volume: %1%").arg(val));

    napp->player()->setVolume(val);
}

void WaSkin::slotPlaying()
{
    waStatus->setStatus(STATUS_PLAYING);

    if (!napp->playlist()->current()) {
      return;
    }

    fileInfo info(napp->playlist()->current());

    if (!info.bps())
        waBPS->setText("");
    else
        waBPS->setText(QString::number(info.bps()));

    if (!info.KHz())
        waFreq->setText("");
    else
        waFreq->setText(QString::number(info.KHz() / 1000));

    setChannels(info.channelCount());
    guiSpectrumAnalyser->resumeVisualization();

    if (napp->player()->getLength() == -1)
        waJumpSlider->hide();
    else
        waJumpSlider->show();

    timetick();
}

void WaSkin::slotStopped()
{
    waStatus->setStatus(STATUS_STOPPED);

    waDigit->setTime("");

    waBPS->setText("");
    waFreq->setText("");
    setChannels(0);

    waJumpSlider->setJumpValue(0);
    // -1 == disable jump bar
    waJumpSlider->setJumpRange(-1);


    mJumpPressed = false;
    waJumpSlider->cancelDrag();

    waJumpSlider->hide();

    guiSpectrumAnalyser->pauseVisualization();
}

void WaSkin::slotPaused()
{
    waStatus->setStatus(STATUS_PAUSED);
}

void WaSkin::keyPressEvent(QKeyEvent *e) {
    switch(e->key()) {
    case Key_Up:
        napp->player()->setVolume(napp->player()->volume() + 5);
        break;
    case Key_Down:
        napp->player()->setVolume(napp->player()->volume() - 5);
        break;
    case Key_Left:
        if (napp->player()->current())
            napp->player()->skipTo(napp->player()->getTime() - 5000);

        break;
    case Key_Right:
        if (napp->player()->current())
            napp->player()->skipTo(napp->player()->getTime() + 5000);

        break;

    case Key_Z:
        napp->player()->back();
        break;

    case Key_X:
        if (napp->player()->isPaused())
	     napp->player()->playpause();
        else
	     napp->player()->playCurrent();

        break;

    case Key_C:
        if (!napp->player()->isStopped())
	    napp->player()->playpause();

        break;

    case Key_V:
        napp->player()->stop();
        break;

    case Key_B:
        napp->player()->forward();
        break;

    case Key_R:
        repeat->setToggled(!repeat->toggled());
        updateLoopStyle();
        break;

    case Key_S:
        shuffle->setToggled(!shuffle->toggled());
        updateLoopStyle();
        break;
    }
}

QString WaSkin::defaultSkin() {
    return "Winamp";
}

#include "waSkin.moc"
