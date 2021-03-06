/* -------------------------------------------------------------------------
   -- kmahjongg timer. Based on a slightly modified verion of the QT demo --
   -- program dclock. Copyright as shown below.                           --
   ------------------------------------------------------------------------- */

/****************************************************************************
** $Id: GameTimer.h 650394 2007-04-04 12:46:34Z mueller $
**
** Copyright (C) 1992-1998 Troll Tech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef KM_GAME_TIMER 
#define KM_GAME_TIMER 

#include <qlcdnumber.h>
#include <qdatetime.h>

enum TimerMode {running = -53 , stopped= -54 , paused = -55};

class GameTimer: public QLCDNumber 
{
    Q_OBJECT
public:
    GameTimer( QWidget *parent=0, const char *name=0 );

    int toInt(); 
    QString toString() {return theTimer.toString();}	
    void fromString(const char *);

protected:					// event handlers
    void	timerEvent( QTimerEvent * );
 
public slots:
    void start();
    void stop();
    void pause();


private slots:					// internal slots
    void	showTime();

private:					// internal data
    bool	showingColon;
    QTime	theTimer;
    TimerMode   timerMode;
};


#endif 
