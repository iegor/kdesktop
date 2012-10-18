/***************************************************************************
    begin                : Mon Jun 17 2002
    copyright            : (C) 2002 by Arnold Krille
    email                : arnold@arnoldarts.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 ***************************************************************************/

#ifndef KRECORD_H
#define KRECORD_H

#include <kmainwindow.h>

class KRecPrivate;

/// KDE Recording GUI.

/**
	Presents a GUI for Recording.

	Supports various takes, Play-Thru and Playing of previous takes.

	@author Arnold Krille
	*/

class KRecord : public KMainWindow  {
   Q_OBJECT
public:
	/// Constructor
	KRecord( QWidget* /*parent*/=0, const char* /*name*/=0 );
	/// Destructor
	~KRecord();
public slots:
	/// Does nothing.
	void nothing() { }
	/// Starts Recording.
	void startRec();
	/// Starts Playback.
	void startPlay();
	/// Stops Recording.
	void stopRec();

protected slots:
	/// Checks all the Actions and sets Enabled/Disabled.
	//void checkActions();
protected:
	bool queryClose();
private:
	KRecPrivate *d;
};

#endif

// vim:sw=4:ts=4
