/*
  Winamp Skin
  Copyright (C) 1999  Martin Vogt
  Copyright (C) 2001  Ryan Cumming

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __WASKIN_H
#define __WASKIN_H

#include <noatun/plugin.h>
#include <noatun/app.h>
#include <noatun/player.h>
#include <qwidget.h>

// Use forward declarations so we compile in a finite time
class WaSkinModel;
class WaSkinManager;

class WaButton;
class WaDigit;
class WaLabel;
class WaInfo;
class WaMain;
class WaIndicator;
class WaTitleBar;
class WaClutterbar;
class WaStatus;

class WaJumpSlider;
class WaVolumeSlider;
class WaBalanceSlider;

class GuiSpectrumAnalyser;

class WaSkin;
extern WaSkin *_waskin_instance;

class WaSkin : public QWidget, public UserInterface {
  Q_OBJECT 
  NOATUNPLUGIND
    WaSkinModel *waSkinModel;
    WaSkinManager *waSkinManager;

  public:
     WaSkin();
    ~WaSkin();

    int getSkinId();

    void loadSkin(QString skinDir);
    void setChannels(int val);

    QSize sizeHint() const;

    static QString defaultSkin();
    static WaSkin *instance() { return _waskin_instance; }

  public slots:
    void repeatClickedEvent(bool);
    void shuffleClickedEvent(bool);
    void playlistClickedEvent(bool);
    void eqClickedEvent();

    // seek bar
    void jump(int second);
    void jumpSliderPressed();
    void jumpSliderReleased();
    void jumpValueChanged(int);

    void menuEvent();
    void minimizeEvent();
    void shadeEvent();
    void doUnload();
    void doClose();

    // balance
    void balanceSliderPressed();
    void balanceSetValue(int val);
    void balanceSliderReleased();

    // volume
    void volumeSliderPressed();
    void volumeSetValue(int val);
    void volumeSliderReleased();

    void playCurrentEvent();
    void playPauseEvent();

    void loopChange(int loopType);
    void playlistShown();
    void playlistHidden();
    void newSong();

    void timetick();
    void digitsClicked();

    void slotPlaying();
    void slotStopped();
    void slotPaused();

    WaSkinModel *skinModel() { return waSkinModel; }
    WaSkinManager *skinManager() { return waSkinManager; }
    WaInfo *skinInfo() { return waInfo; }
    
  protected:
    void updateLoopStyle();

    void createButtons();
    void createHighLevelElements();

    void keyPressEvent(QKeyEvent *);

    void shade();
    void unshade();

    QString getTitleString();
    QString getTimeString(int milliseconds, bool truncate = false);

    void focusOutEvent ( QFocusEvent * );
    void focusInEvent ( QFocusEvent * );

    void dragEnterEvent(QDragEnterEvent * event);
    void dropEvent(QDropEvent * event);

    void wheelEvent(QWheelEvent *e);

    WaButton *prev;
    WaButton *play;
    WaButton *pause;
    WaButton *stop;
    WaButton *next;
    WaButton *eject;
    WaButton *shuffle;
    WaButton *repeat;
    WaButton *playlist;
    WaButton *eq;

    WaButton *menu;
    WaButton *minimize;
    WaButton *titleshade;
    WaButton *close;

    WaJumpSlider *waJumpSlider;
    WaVolumeSlider *waVolumeSlider;
    WaBalanceSlider *waBalanceSlider;
    WaDigit *waDigit;

    WaLabel *waBPS;
    WaLabel *waFreq;

    WaInfo *waInfo;
    WaStatus *waStatus;
    WaIndicator *waStereo;
    WaIndicator *waMono;

    WaMain *main;
    WaTitleBar *waTitleBar;
    WaClutterbar *waClutterbar;

    GuiSpectrumAnalyser *guiSpectrumAnalyser;

    bool title_shaded;

    bool mJumpPressed;
    bool mBalancePressed;
    bool mVolumePressed;
};
#endif
