/*
 * video.cpp
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qpainter.h>
#include <qtimer.h>
#include <qcursor.h>

#include <kstatusbar.h>
#include <ktoolbar.h>
#include <kmenubar.h>
#include <kdebug.h>

#include <gst/interfaces/xoverlay.h>

#include "video.h"

#include "video.moc"



VideoWindow::VideoWindow( QWidget *parent, GstElement *_element )
		: QWidget (parent), m_width(0), m_height(0), m_aspectRatio(AUTO)
{
	m_play = NULL;

	gst_object_ref( GST_OBJECT (_element) );
	m_element = _element;

	setPaletteBackgroundColor(QColor(0,0,0)); //black
	setUpdatesEnabled(false);

	connect( &m_mouseHideTimer, SIGNAL(timeout()), this, SLOT(slotHideMouse()) );
}



void VideoWindow::setPlaybin( GstElement *play )
{
	m_play = play;
}



VideoWindow::~VideoWindow()
{
	if ( m_element && GST_IS_X_OVERLAY(m_element) ) {
		gst_x_overlay_set_xwindow_id( GST_X_OVERLAY(m_element), 0 );
	}

	gst_object_unref( GST_OBJECT(m_element) );
	kdDebug() << "VideoWindow: destructed" << endl;
}



void VideoWindow::refresh()
{
	/* expose overlay */
	if ( m_element && GST_IS_X_OVERLAY(m_element) ) {
		gst_x_overlay_set_xwindow_id( GST_X_OVERLAY (m_element), winId() );
		gst_x_overlay_expose( GST_X_OVERLAY (m_element) );
	}
}



void VideoWindow::paintEvent(QPaintEvent */*event*/)
{
	refresh();
}



void VideoWindow::mousePressEvent(QMouseEvent* mev)
{
	if ( mev->button()==Qt::RightButton ) {
		emit signalRightClick( mev->globalPos() );
		mev->accept();
	}
	else {
		mev->ignore();
	}
}



void VideoWindow::newCapsset( const GstCaps *caps )
{
	const GstStructure *s;
	m_width = 0;
	m_height = 0;

	s = gst_caps_get_structure(caps, 0);
	if (s) 	{
		const GValue *par;

		gst_structure_get_int(s, "width", &m_width);
		gst_structure_get_int(s, "height", &m_height);
		if ( (par = gst_structure_get_value (s, "pixel-aspect-ratio")) ) {
			int num = gst_value_get_fraction_numerator(par),
			          den = gst_value_get_fraction_denominator(par);

			if (num > den)
				m_width = (int) ((float) num * m_width / den);
			else
				m_height = (int) ((float) den * m_height / num);
		}
	}

	QSize frame = getFrameSize();
	correctByAspectRatio( frame );
	emit signalNewFrameSize( frame );
	//set correct geometry
	setGeometry();
}



void VideoWindow::newState()
{
	if ( !m_play )
		return;

	const GList *list = NULL;

	g_object_get( G_OBJECT(m_play), "stream-info", &list, NULL );
	for ( ; list != NULL; list = list->next ) {
		GObject *info = (GObject *)list->data;
		gint type;
		GParamSpec *pspec;
		GEnumValue *val;
		GstPad *pad = NULL;

		if ( !info )
			continue;

		g_object_get( info, "type", &type, NULL );
		pspec = g_object_class_find_property( G_OBJECT_GET_CLASS (info), "type" );
		val = g_enum_get_value( G_PARAM_SPEC_ENUM(pspec)->enum_class, type );

		if ( !g_strcasecmp(val->value_nick, "video") ) {
			GstCaps *caps;
			g_object_get( info, "object", &pad, NULL );
			if ( (caps=gst_pad_get_negotiated_caps(pad)) ) {
				newCapsset( caps );
				gst_caps_unref (caps);
				return;
			}
		}
	}
	m_width = 0;
	m_height = 0;
	QSize frame = getFrameSize();
	correctByAspectRatio( frame );
	emit signalNewFrameSize( frame );
	//set correct geometry
	setGeometry();
}



void VideoWindow::setGeometry( int, int, int, int )
{
	QSize frame = getFrameSize();
	QSize window = parentWidget()->size();
	int x = 0, y = 0, width = 0, height = 0;

	if ( frame.width() == 0 || frame.height() == 0 ) {
		QWidget::setGeometry(0, 0, window.width(), window.height());
		return;
	}

	correctByAspectRatio( frame );

	float frameAspect = (float)frame.width() / (float)frame.height();
	float windowAspect = (float)window.width() / (float)window.height();

	if ( frameAspect >= windowAspect ) {
		width = window.width();
		height = (int)((float)width / frameAspect);
		y = (window.height() - height) / 2;
	}
	else {
		height = window.height();
		width = (int)((float)height * frameAspect);
		x = (window.width() - width) / 2;
	}

	//kdDebug() << "VideoWindow::setGeometry: " << x << " : " << y << " : " << width << " : " << height << endl;
	QWidget::setGeometry( x, y, width, height );
}



void VideoWindow::setGeometry()
{
	setGeometry(0, 0, 0, 0);
}



void VideoWindow::correctByAspectRatio( QSize& frame )
{
	float factor = 0;

	switch ( m_aspectRatio ) {
		case AUTO: return;
		case FOURBYTHREE: factor = 4.0 / 3.0; break;
		case ANAMORPHIC: factor = 16.0 / 9.0; break;
		case DVB:	factor = 2.11; break;
		case SQUARE: factor = 1.0; break;
	}

	float frameAspect = (float)frame.width() / (float)frame.height();
	factor = factor / frameAspect;
	if ( factor > 1.0 )
		frame.setWidth((int)((float)frame.width() * factor));
	else
		frame.setHeight((int)((float)frame.height() / factor));
}



void VideoWindow::slotAspectRatioAuto()
{
	m_aspectRatio = AUTO;
	QSize frame = getFrameSize();
	correctByAspectRatio( frame );
	emit signalNewFrameSize( frame );
	setGeometry();
}



void VideoWindow::slotAspectRatio4_3()
{
	m_aspectRatio = FOURBYTHREE;
	QSize frame = getFrameSize();
	correctByAspectRatio( frame );
	emit signalNewFrameSize( frame );
	setGeometry();
}



void VideoWindow::slotAspectRatioAnamorphic()
{
	m_aspectRatio = ANAMORPHIC;
	QSize frame = getFrameSize();
	correctByAspectRatio( frame );
	emit signalNewFrameSize( frame );
	setGeometry();
}



void VideoWindow::slotAspectRatioDVB()
{
	m_aspectRatio = DVB;
	QSize frame = getFrameSize();
	correctByAspectRatio( frame );
	emit signalNewFrameSize( frame );
	setGeometry();
}



void VideoWindow::slotAspectRatioSquare()
{
	m_aspectRatio = SQUARE;
	QSize frame = getFrameSize();
	correctByAspectRatio( frame );
	emit signalNewFrameSize( frame );
	setGeometry();
}

/******** mouse cursor hiding on fullscreen ****/

void VideoWindow::startMouseHideTimer()
{
	m_mouseHideTimer.start( 5000 );
	setMouseTracking( true );
}



void VideoWindow::stopMouseHideTimer()
{
	m_mouseHideTimer.stop();
	setMouseTracking( false );
	setCursor( QCursor(Qt::ArrowCursor) );
}



void VideoWindow::slotHideMouse()
{
	setCursor( QCursor(Qt::BlankCursor) );
}



void VideoWindow::mouseMoveEvent( QMouseEvent *mev )
{
	if ( cursor().shape() == Qt::BlankCursor )
		setCursor( QCursor(Qt::ArrowCursor) );
	mev->ignore();
}
