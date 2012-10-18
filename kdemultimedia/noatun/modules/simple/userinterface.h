/*
   This file is part of KDE/aRts (Noatun) - xine integration
   Copyright (C) 2002 Ewald Snel <ewald@rambo.its.tudelft.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
*/

#ifndef __USERINTERFACE_H
#define __USERINTERFACE_H

#include <noatun/controls.h>
#include <noatun/plugin.h>
#include <noatun/video.h>
#include <kaction.h>
#include <kmainwindow.h>
#include <kmedia2.h>
#include <qevent.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qvbox.h>
#include "propertiesdialog.h"


class SimpleUI : public KMainWindow, public UserInterface
{
    Q_OBJECT

public:
    SimpleUI();
    ~SimpleUI();

protected:
    void setupActions();
    void setupCentralWidget();

    virtual void closeEvent( QCloseEvent * );
    virtual void dragEnterEvent( QDragEnterEvent *event );
    virtual void dropEvent( QDropEvent *event );

public slots:
    void slotAdaptSize( int width, int height );
    void slotPlaying();
    void slotStopped();
    void slotPaused();
    void slotTimeout();
    void slotSkipTo( int sec );
    void slotChanged();
    void slotContextMenu( const QPoint &pos );

private slots:
    void slotSliderMoved( int sec );
    void slotVolumeSliderMoved( int volume );
    void slotVolumeFrame();
    void slotVolumeChanged( int volume );
    void showMenubar();
    void showStatusbar();

private:
    PropertiesDialog *propertiesDialog;
    QPopupMenu *contextMenu;
    QPushButton *stopButton;
    QPushButton *playButton;
    QPushButton *volumeButton;
    QVBox *volumeFrame;
    QLabel *volumeLabel;
    QLabel *positionLabel;
    VideoFrame *video;
    L33tSlider *volumeSlider;
    L33tSlider *slider;
    int extra_width;
    int extra_height;
    KToggleAction *menubarAction;
    KToggleAction *statusbarAction;
};

#endif
