/*
	Copyright (C) 2001 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/* $Id: wavecapture.h 465369 2005-09-29 14:33:08Z mueller $ */

#ifndef WAVECAPTURE_H
#define WAVECAPTURE_H

#include <noatun/plugin.h>
#include <artsmodules.h>

#include <string>

class QTimer;
namespace KIO { class Job; }

class WaveCapture : public QObject, public Plugin
{
Q_OBJECT
NOATUNPLUGIND

public:
	WaveCapture();
	virtual ~WaveCapture();

	void init();

protected slots:
	void toggle();
	void newSong();
	void stopped();
	void playing();
	void paused();
	void saveAs();
	void copyFinished( KIO::Job* );

private:
	void start();
	void stop();

	int pluginMenuItem;

	bool _enabled;
	enum Status { PAUSED, STOPPED, PLAYING };
	Status _status;

	Arts::Effect_WAVECAPTURE _capture;
	int _count;
	long _id;
	std::string _filename;

	QTimer* _timer;
	KIO::Job* m_job;
};

#endif

// vim:ts=4:sw=4
