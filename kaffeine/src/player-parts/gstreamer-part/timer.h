/*
 * timer.h
 *
 * Copyright (C) 2006 Christophe Thommeret <hftom@free.fr>
 * Copyright (C) 2004-2005 Jürgen Kofler <kaffeine@gmx.net>
 *
 * heavily based on kiss by Ronald Bultje <rbultje@ronald.bitfreak.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef TIMER_H
#define TIMER_H

#include <qobject.h>

#include <gst/gst.h>

class QSlider;
class QLabel;
class QTimer;
class QTime;



class Timer : public QObject
{
	Q_OBJECT

public:
	Timer();
	~Timer ();

	void start();
	void stop();
	void setPlaybin( GstElement *play );

	long getTotalTimeMS() { return m_totalTimeMS; }
	long getCurrentTimeMS() { return m_currentTimeMS; }

	void seekPercent(uint percent);

	QLabel* getLabel() const;
	QSlider* getSlider() const;

public slots:
	void slotSeekStart();
	void slotSeek();

private slots:
	void slotUpdate();

private:
	QTimer updateTimer;
	QLabel *m_label;
	QSlider *m_slider;
	GstElement *m_play;
	bool seeking;

	long m_currentTimeMS;
	long m_totalTimeMS;
	guint64 m_len, m_pos;
};

#endif /* TIMER_H */
