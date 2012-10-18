/*  This file is part of the KDE libraries
    Copyright (C) 2002 Simon MacMullen
    Copyright (C) 2003 Ewald Snel <ewald@rambo.its.tudelft.nl>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

// $Id: videocreator.cpp 461357 2005-09-17 12:31:20Z woebbe $

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>

#include <qpixmap.h>
#include <qdialog.h>
#include <qfile.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpaintdevice.h>

#include <iostream>

#include <kstandarddirs.h>
#include <kapplication.h>

#define XINE_ENABLE_EXPERIMENTAL_FEATURES	1

#include <xine.h>

#include "videocreator.h"
#include "videoscaler.h"

#define TIMEOUT		15	// 15 seconds
#define MAX_ATTEMPTS	25


// Global xine pointer
static xine_t		*xine_shared	 = NULL;
static pthread_mutex_t	 xine_mutex	 = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t	 xine_cond	 = PTHREAD_COND_INITIALIZER;
static int		 xineRefCount	 = 0;

static void xine_init_routine()
{
    char cfgFileName[272];

    xine_shared = (xine_t *)xine_new();

    snprintf( cfgFileName, 272, "%s/.xine/config", getenv( "HOME" ) );

    xine_config_load( xine_shared, (const char *)cfgFileName );

    xine_init( xine_shared );
}

static void *xine_timeout_routine( void * )
{
    pthread_mutex_lock( &xine_mutex );

    while (xine_shared != 0)
    {
	if (xineRefCount == 0)
	{
	    struct timespec ts;
	    struct timeval tv;

	    gettimeofday( &tv, 0 );

	    ts.tv_sec	= tv.tv_sec;
	    ts.tv_nsec	= tv.tv_usec * 1000;
	    ts.tv_sec  += TIMEOUT;

	    if (pthread_cond_timedwait( &xine_cond, &xine_mutex, &ts ) != 0 &&
		xineRefCount == 0)
	    {
		xine_exit( xine_shared );
		xine_shared = NULL;
		break;
	    }
	}
	else
	{
	    pthread_cond_wait( &xine_cond, &xine_mutex );
	}
    }
    pthread_mutex_unlock( &xine_mutex );

    return NULL;
}

static xine_t *xine_shared_init()
{
    pthread_mutex_lock( &xine_mutex );

    ++xineRefCount;

    if (xine_shared == 0)
    {
	pthread_t thread;

	xine_init_routine();

	if (pthread_create( &thread, NULL, xine_timeout_routine, NULL ) == 0)
	{
	    pthread_detach( thread );
	}
    }
    else
    {
	pthread_cond_signal( &xine_cond );
    }
    pthread_mutex_unlock( &xine_mutex );

    return xine_shared;
}

static void xine_shared_exit( xine_t * )
{
    pthread_mutex_lock( &xine_mutex );

    if (--xineRefCount == 0)
    {
	pthread_cond_signal( &xine_cond );
    }
    pthread_mutex_unlock( &xine_mutex );
}

static QImage createThumbnail( xine_video_frame_t *frame, int width, int height )
{
    unsigned char *base[3];
    unsigned int pitches[3];

    if ((frame->aspect_ratio * height) > width)
	height = (int)(.5 + (width / frame->aspect_ratio));
    else
	width = (int)(.5 + (height * frame->aspect_ratio));

    QImage image( width, height, 32 );

    if (frame->colorspace == XINE_IMGFMT_YV12)
    {
	int y_size, uv_size;

	pitches[0] = (frame->width + 7) & ~0x7;
	pitches[1] = (((frame->width + 1) / 2) + 7) & ~0x7;
	pitches[2] = pitches[1];

	y_size  = pitches[0] * frame->height;
	uv_size = pitches[1] * ((frame->height + 1) / 2);

	base[0] = frame->data;
	base[1] = base[0] + y_size + uv_size;
	base[2] = base[0] + y_size;

	scaleYuvToRgb32( frame->width, frame->height, base, pitches,
			 width, height, (unsigned int *)image.bits(),
			 image.bytesPerLine() );
    }
    else if (frame->colorspace == XINE_IMGFMT_YUY2)
    {
	pitches[0] = 2*((frame->width + 3) & ~0x3);
	base[0] = frame->data;

	scaleYuy2ToRgb32( frame->width, frame->height, base[0], pitches[0],
			  width, height, (unsigned int *)image.bits(),
			  image.bytesPerLine() );
    }
    return image;
}

// Return the variance of the brightness of the pixels
static double imageVariance( unsigned char *pixels, int pitch,
			     int width, int height, int step )
{
    double sigmaX = 0;
    double sigmaXSquared = 0;

    for (int y=0; y < height ; y++)
    {
	unsigned int uSigmaX = 0;
	unsigned int uSigmaXSquared = 0;

	for (int x=0, n=(width * step); x < n ; x+=step)
	{
	    int gray = pixels[x];

	    uSigmaX += gray;
	    uSigmaXSquared += gray * gray;
        }

	sigmaX += uSigmaX;
	sigmaXSquared += uSigmaXSquared;

	pixels += pitch;
    }

    unsigned int total = height * width;

    return sqrt( sigmaXSquared / total - (sigmaX / total) * (sigmaX / total) );
}

static bool findBestFrame( xine_video_port_t *vo_port, xine_video_frame_t *frame )
{
    xine_video_frame_t frames[2], *bestFrame = NULL;
    double variance, bestVariance = 0;

    for (int i=0, n=0; i < MAX_ATTEMPTS; i++)
    {
	xine_video_frame_t *cFrame = &frames[n];

	// Try to read next frame
	if (!xine_get_next_video_frame( vo_port, cFrame ))
	{
	    break;
	}

	variance = imageVariance( cFrame->data, ((cFrame->width + 7) & ~0x7),
				  cFrame->width, cFrame->height,
				 (cFrame->colorspace == XINE_IMGFMT_YV12) ? 1 : 2 );

	// Compare current frame to best frame
	if (bestFrame == NULL || variance > bestVariance)
	{
	    if (bestFrame != NULL)
	    {
		xine_free_video_frame( vo_port, bestFrame );
	    }

	    bestFrame = cFrame;
	    bestVariance = variance;

	    n = (1 - n);
	}
	else
	{
	    xine_free_video_frame( vo_port, cFrame );
	}

	// Stop searching if current frame is interesting enough
	if (variance > 40.0)
	{
	    break;
	}
    }

    // This should be the best frame to create a thumbnail from
    if (bestFrame != NULL)
    {
	*frame = *bestFrame;
    }
    return (bestFrame != NULL);
}


extern "C"
{
    ThumbCreator *new_creator()
    {
        return new VideoCreator;
    }
}

VideoCreator::VideoCreator()
{
}

VideoCreator::~VideoCreator()
{
}

bool VideoCreator::create(const QString &path, int width, int height, QImage &img)
{
    if (m_sprocketSmall.isNull())
    {
        QString pixmap = locate( "data", "videothumbnail/sprocket-small.png" );
        m_sprocketSmall = QPixmap(pixmap);
        pixmap = locate( "data", "videothumbnail/sprocket-medium.png" );
        m_sprocketMedium = QPixmap(pixmap);
        pixmap = locate( "data", "videothumbnail/sprocket-large.png" );
        m_sprocketLarge = QPixmap(pixmap);
    }

    // The long term plan is to seek to frame 1, create thumbnail, see if is is
    // interesting enough, if not seek to frame 2, then 4, then 8, etc.
    // "Interesting enough" means the variance of the pixel brightness is high. This
    // is because many videos fade up from black and a black rectangle is boring.
    //
    // But for the time being we can't seek so we just let it play for one second 
    // then take whatever we find.

    xine_t *xine = xine_shared_init();
    xine_audio_port_t *ao_port = xine_new_framegrab_audio_port( xine );
    xine_video_port_t *vo_port = xine_new_framegrab_video_port( xine );
    xine_stream_t *stream = xine_stream_new( xine, ao_port, vo_port );
    bool success = false;

    if (xine_open( stream, QFile::encodeName ( path ).data() ))
    {
	xine_video_frame_t frame;
	int length;

	// Find 'best' (or at least any) frame
	if (!xine_get_pos_length( stream, NULL, NULL, &length ) || length > 5000)
	{
	    if (xine_play( stream, 0, 4000 ))
	    {
		success = findBestFrame( vo_port, &frame );
	    }
	}
	if (!success)
	{
	    // Some codecs can't seek to start, but close/open works
	    xine_close( stream );
	    xine_open( stream, path.ascii() );

	    if (xine_play( stream, 0, 0 ))
	    {
		success = findBestFrame( vo_port, &frame );
	    }
	}

	// Create thumbnail image
	if (success)
	{
	    QPixmap pix( createThumbnail( &frame, width, height ) );
	    QPainter painter( &pix );
	    QPixmap sprocket;

	    if (pix.height() < 60)
		sprocket = m_sprocketSmall;
	    else if (pix.height() < 90)
		sprocket = m_sprocketMedium;
	    else
		sprocket = m_sprocketLarge;

	    for (int y = 0; y < pix.height() + sprocket.height(); y += sprocket.height()) {
		painter.drawPixmap( 0, y, sprocket );
	    }

	    img = pix.convertToImage();

	    xine_free_video_frame( vo_port, &frame );
	}

	xine_stop( stream );
    }

    xine_dispose( stream );
    xine_close_audio_driver( xine, ao_port );
    xine_close_video_driver( xine, vo_port );
    xine_shared_exit( xine );

    return (success);
}

ThumbCreator::Flags VideoCreator::flags() const
{
    return (ThumbCreator::Flags) (DrawFrame);
}

#include "videocreator.moc"
