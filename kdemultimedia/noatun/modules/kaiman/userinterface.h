/*
   Copyright (c) 2000 Stefan Schimanski <1Stein@gmx.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   aint with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <noatun/plugin.h>
#include <noatun/app.h>
#include <kmainwindow.h>
#include "style.h"


class Player;

/**
 * @short Main window class
 * @author Stefan Schimanski <1Stein@gmx.de>
 * @version 0.1
 */
class Kaiman : public KMainWindow, public UserInterface
{
Q_OBJECT
 public:
    Kaiman();
    virtual ~Kaiman();

    bool changeStyle( const QString &style, const QString &desc=QString::null );
 public slots:
    void dropEvent( QDropEvent * );
    void doDropEvent( QDropEvent * );
    void dragEnterEvent( QDragEnterEvent * );
    void closeEvent(QCloseEvent*);

 protected slots:
    void seekStart( int );
    void seekDrag( int );
    void seekStop( int );
    void seek( int );
    void toggleSkin();

    void setVolume( int vol );
    void volumeUp();
    void volumeDown();

    void execMixer();


    void timeout();
    void loopTypeChange( int t );
    void newSongLen( int mins, int sec );
    void newSong();
    void updateMode();
    void toggleLoop();
    void toggleShuffle();

 public:
    static const char DEFAULT_SKIN[];
    static Kaiman *kaiman;

 protected:
    bool loadStyle( const QString &style, const QString &desc );


    bool _seeking;
    bool _altSkin;
    KaimanStyle *_style;
};


#endif
