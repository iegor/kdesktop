/*
 * video.h
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

#ifndef VIDEO_H
#define VIDEO_H

#include <qwidget.h>
#include <kapplication.h>

#include <qsize.h>

#include <gst/gst.h>



class VideoWindow : public QWidget
{
	Q_OBJECT

public:
	VideoWindow( QWidget *parent, GstElement *element );
	~VideoWindow ();

	enum AspectRatio {
		AUTO,
		FOURBYTHREE,
		ANAMORPHIC,
		DVB,
		SQUARE
	};

	void newState();
	void newCapsset( const GstCaps *caps );
	void setPlaybin( GstElement *play );

	void refresh();

	QSize getFrameSize() { return QSize(m_width, m_height); }

	void startMouseHideTimer();
	void stopMouseHideTimer();

signals:
	void signalNewFrameSize( const QSize& );
	void signalRightClick( const QPoint& );

public slots:
// 	void setGeometry(const QRect&);
	void setGeometry();
	void setGeometry( int x, int y, int width, int heigth );
	void slotAspectRatioAuto();
	void slotAspectRatio4_3();
	void slotAspectRatioAnamorphic();
	void slotAspectRatioDVB();
	void slotAspectRatioSquare();

private slots:
	void slotHideMouse();

protected:
	void paintEvent( QPaintEvent *event );
	void mousePressEvent( QMouseEvent* );
	void mouseMoveEvent( QMouseEvent* );

private:
	void correctByAspectRatio( QSize &frame );

private:
	GstElement *m_element, *m_play;
	int m_width, m_height;
	AspectRatio m_aspectRatio;
	QTimer m_mouseHideTimer;
};

#endif /* VIDEO_H */
