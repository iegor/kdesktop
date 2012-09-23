/*
 * timer.cpp
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

#include <klocale.h>
#include <kdebug.h>

#include <qslider.h>
#include <qlabel.h>
#include <qtimer.h>

#include "timer.h"

#include "timer.moc"



Timer::Timer() : QObject()
{
	m_label = new QLabel( "0:00 / 0:00", 0 );
	m_slider = new QSlider( Qt::Horizontal, 0 );
	m_slider->setMinValue( 0 );
	m_slider->setEnabled( false );

	connect( &updateTimer, SIGNAL(timeout()), SLOT(slotUpdate()) );

	m_pos = GST_CLOCK_TIME_NONE;
	m_len = GST_CLOCK_TIME_NONE;

	m_play = NULL;
	seeking = false;

	connect( m_slider, SIGNAL(sliderPressed()), this, SLOT(slotSeekStart()) );
	connect( m_slider, SIGNAL(sliderReleased()), this, SLOT(slotSeek()) );
}



void Timer::setPlaybin( GstElement *play )
{
	m_play = play;
	m_slider->setEnabled( false );
	m_slider->setValue( 0 );
}



Timer::~Timer()
{
}



static char *niceTime( guint64 t )
{
	if ( t >= GST_SECOND*60*60 ) {
		return g_strdup_printf("%d:%02d:%02d",
		                        (int) (t / (GST_SECOND * 60 * 60)),
		                        (int) ((t / (GST_SECOND * 60)) % 60),
		                        (int) ((t / GST_SECOND) % 60));
	}
	else {
		return g_strdup_printf("%d:%02d",
		                        (int) ((t / (GST_SECOND * 60)) % 60),
		                        (int) ((t / GST_SECOND) % 60));
	}
}



void Timer::slotSeekStart()
{
	seeking = true;
}



void Timer::slotSeek()
{
	if ( !m_play )
		return;

	gint64 gval = m_slider->value();
	GstSeekFlags flags = (GstSeekFlags)(GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_KEY_UNIT);
	gst_element_seek( m_play, 1.0, GST_FORMAT_TIME, flags,
		GST_SEEK_TYPE_SET, gval*GST_SECOND, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
	gst_element_get_state( m_play, NULL, NULL, 100*GST_MSECOND);
	seeking = false;
}



void Timer::seekPercent( uint percent )
{
	//slotSeek( (m_slider->maxValue() * percent) / 100 );
}



void Timer::slotUpdate()
{
	gint64 t;
	GstFormat fmt = GST_FORMAT_TIME;
	char *txt;

	if ( seeking )
		return;

	if ( !m_play )
		return;

	if ( gst_element_query_duration( m_play, &fmt, &t ) ) {
		m_len = t;
		m_slider->setMaxValue( m_len / GST_SECOND );
	}

	if ( !gst_element_query_position( m_play, &fmt, &t ) )
		return;

	m_pos = t;

	m_currentTimeMS = m_pos/1000000;
	m_totalTimeMS = m_len/1000000;

	if ( GST_CLOCK_TIME_IS_VALID (m_len) ) {
		gchar *t1 = niceTime( m_pos ), *t2 = niceTime( m_len );
		txt = g_strdup_printf( "%s / %s", t1, t2) ;
		g_free(t1);
		g_free(t2);
	}
	else {
		txt = niceTime( m_pos );
	}
	m_label->setText( txt );
	g_free( txt );

	m_slider->setValue( m_pos / GST_SECOND );
}



void Timer::start()
{
	if ( updateTimer.isActive() )
		return;
	m_slider->setEnabled( true);
	seeking = false;
	updateTimer.start( 1000 );
}



void Timer::stop()
{
	if ( !updateTimer.isActive() )
		return;
	updateTimer.stop();
	m_slider->setEnabled( false );
	m_slider->setValue(0);
}



QLabel* Timer::getLabel() const
{
	return m_label;
}



QSlider* Timer::getSlider() const
{
	return m_slider;
}
