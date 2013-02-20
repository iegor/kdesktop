/***************************************************************************
                          Interface to access XMMS
                             -------------------
    begin                : Tue Apr 25 11:53:11 CEST 2000
    copyright            : (C) 2000-2002 by Stefan Gehn
    email                : metz {AT} gehn {DOT} net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_XMMS // only use if there's xmms installed on compiletime

#ifndef XMMSINTERFACE_H
#define XMMSINTERFACE_H

#include "playerInterface.h"
#include <qtimer.h>

class XmmsInterface : public PlayerInterface
{
	Q_OBJECT
	public:
		XmmsInterface();
		~XmmsInterface();

	public slots:
		virtual void updateSlider();
		virtual void sliderStartDrag();
		virtual void sliderStopDrag();
		virtual void jumpToTime(int msec);
		virtual void playpause();
		virtual void stop();
		virtual void next();
		virtual void prev();
		virtual void volumeUp();
		virtual void volumeDown();
		virtual void dragEnterEvent(QDragEnterEvent* event);
		virtual void dropEvent(QDropEvent* event);
		virtual const QString getTrackTitle() const;
		virtual int playingStatus();

	private:
		QTimer *xmms_timer;
		int timervalue;
		bool bStartingXMMS;
};
#endif // XMMSINTERFACE_H
#endif // HAVE_XMMS
