/*
   This file is part of KDE/aRts (Noatun) - xine integration
   Copyright (C) 2002-2003 Ewald Snel <ewald@rambo.its.tudelft.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <xine/audio_out.h>

#include "audio_fifo_out.h"

#define GAP_TOLERANCE	5000

typedef struct fifo_driver_s
{
    ao_driver_t	     ao_driver;

    xine_arts_audio *audio;

    int		     capabilities;
    int		     mode;
    pthread_mutex_t  read_mutex;
    pthread_mutex_t  write_mutex;
    pthread_cond_t   cond;

    uint32_t	     bytes_per_frame;
    uint8_t	    *fifo;
    int		     fifo_size;
    int		     fifo_read_ptr;
    int		     fifo_write_ptr;
    int		     fifo_flush;
    int		     fifo_delay;
} fifo_driver_t;

/*
 * open the audio device for writing to
 */
static int ao_fifo_open( ao_driver_t *this_gen, uint32_t bits, uint32_t rate, int mode )
{
    fifo_driver_t *ao = (fifo_driver_t *)this_gen;

    if ((mode & ao->capabilities) == 0)
    {
	fprintf( stderr, "fifo_audio_out: unsupported mode %08x\n", mode);
	return 0;
    }

    /* lock read buffer */
    pthread_mutex_lock( &ao->read_mutex );

    ao->mode			= mode;
    ao->audio->sample_rate	= rate;
    ao->audio->bits_per_sample	= bits;

    switch (mode)
    {
    case AO_CAP_MODE_MONO:
	ao->audio->num_channels = 1;
	break;
    case AO_CAP_MODE_STEREO:
	ao->audio->num_channels = 2;
	break;
    }

    ao->bytes_per_frame	= (ao->audio->bits_per_sample * ao->audio->num_channels) / 8;
    ao->fifo_size	= ao->audio->sample_rate * ao->bytes_per_frame;
    ao->fifo		= malloc( 2*ao->fifo_size );
    ao->fifo_read_ptr	= 0;
    ao->fifo_write_ptr	= 0;
    ao->fifo_flush	= 0;
    ao->fifo_delay	= 0;

    /* unlock and enable read buffer for aRts sound server */
    pthread_mutex_unlock( &ao->read_mutex );

    return ao->audio->sample_rate;
}

static int ao_fifo_num_channels( ao_driver_t *this_gen )
{
    return ((fifo_driver_t *)this_gen)->audio->num_channels;
}

static int ao_fifo_bytes_per_frame( ao_driver_t *this_gen )
{
    return ((fifo_driver_t *)this_gen)->bytes_per_frame;
}

static int ao_fifo_get_gap_tolerance( ao_driver_t *this_gen )
{
    return GAP_TOLERANCE;
}

static int ao_fifo_bytes_in_buffer( fifo_driver_t *ao )
{
    int bytes_in_buffer = (ao->fifo_write_ptr - ao->fifo_read_ptr);

    if (bytes_in_buffer < 0)
    {
	bytes_in_buffer += ao->fifo_size;
    }
    return bytes_in_buffer;
}

static int ao_fifo_write( ao_driver_t *this_gen, int16_t *data, uint32_t num_frames )
{
    fifo_driver_t *ao = (fifo_driver_t *)this_gen;
    uint8_t *src = (uint8_t *)data;
    int bytes_in_buffer, bytes_to_write, written;

    bytes_to_write = (num_frames * ao->bytes_per_frame);

    pthread_mutex_lock( &ao->write_mutex );

    while (!ao->fifo_flush && bytes_to_write > 0)
    {
	bytes_in_buffer = ao_fifo_bytes_in_buffer( ao );
	written = bytes_to_write;

	if ((bytes_in_buffer + written) >= ao->fifo_size)
	{
	    written  = (ao->fifo_size - bytes_in_buffer - 1);
	    written -= (written % ao->bytes_per_frame);

	    if (written == 0)
	    {
		struct timespec ts;
		struct timeval tv;
		int delay;

		gettimeofday( &tv, 0 );

		delay  = ao_fifo_arts_delay();
		delay += (1000 * num_frames) / ao->audio->sample_rate;
		delay  = (delay < 20) ? 20 : ((delay >= 250) ? 250 : delay + 1);

		ts.tv_sec  = tv.tv_sec + (delay / 1000);
		ts.tv_nsec = (1000 * tv.tv_usec) + (1000000 * (delay % 1000));

		if (ts.tv_nsec >= 1000000000)
		{
		    ts.tv_sec++;
		    ts.tv_nsec -= 1000000000;
		}
		if (pthread_cond_timedwait( &ao->cond, &ao->write_mutex, &ts ) != 0)
		{
		    fprintf( stderr, "fifo_audio_out: blocked for more than %d ms,\n", delay);
		    fprintf( stderr, "fifo_audio_out: %d sample(s) discarded.\n", num_frames);
		    pthread_mutex_unlock( &ao->write_mutex );
		    return 0;
		}
	    }
	}

	if (!ao->fifo_flush && written > 0)
	{
	    int new_write_ptr = (ao->fifo_write_ptr + written);

	    if (new_write_ptr >= ao->fifo_size)
	    {
		new_write_ptr -= ao->fifo_size;

		memcpy( &ao->fifo[ao->fifo_write_ptr], src, (written - new_write_ptr) );
		memcpy( ao->fifo, &src[written - new_write_ptr], new_write_ptr );
	    }
	    else
	    {
		memcpy( &ao->fifo[ao->fifo_write_ptr], src, written );
	    }

	    /* update audio buffer pointer */
	    ao->fifo_write_ptr = new_write_ptr;
	    bytes_to_write -= written;
	    src += written;
	}
    }

    /* audio has stopped */
    ao->fifo_delay += bytes_to_write;

    pthread_mutex_unlock( &ao->write_mutex );

    return 1;
}

static int ao_fifo_delay( ao_driver_t *this_gen )
{
    fifo_driver_t *ao = (fifo_driver_t *)this_gen;
    return (ao_fifo_arts_delay() * ao->audio->sample_rate / 1000)
	   + ((ao_fifo_bytes_in_buffer( ao ) + ao->fifo_delay) / ao->bytes_per_frame);
}

static void ao_fifo_close( ao_driver_t *this_gen )
{
    fifo_driver_t *ao = (fifo_driver_t *)this_gen;

    /* lock read buffer */
    pthread_mutex_lock( &ao->read_mutex );

    /* disable audio driver */
    ao->fifo_flush = 2;
    ao->fifo_delay = 0;

    /* free audio FIFO */
    if (ao->fifo)
    {
	free( ao->fifo );
	ao->fifo = NULL;
    }
    pthread_mutex_unlock( &ao->read_mutex );
}

static uint32_t ao_fifo_get_capabilities( ao_driver_t *this_gen )
{
    return ((fifo_driver_t *)this_gen)->capabilities;
}

static void ao_fifo_exit( ao_driver_t *this_gen )
{
    fifo_driver_t *ao = (fifo_driver_t *)this_gen;

    ao_fifo_close( this_gen );

    pthread_cond_destroy( &ao->cond );
    pthread_mutex_destroy( &ao->read_mutex );
    pthread_mutex_destroy( &ao->write_mutex );

    free( ao );
}

static int ao_fifo_get_property( ao_driver_t *this_gen, int property )
{
    return 0;
}

static int ao_fifo_set_property( ao_driver_t *this_gen, int property, int value )
{
    return ~value;
}

static int ao_fifo_control( ao_driver_t *this_gen, int cmd, ... )
{
    fifo_driver_t *ao = (fifo_driver_t *)this_gen;

    switch (cmd)
    {
    case AO_CTRL_FLUSH_BUFFERS:
    case AO_CTRL_PLAY_PAUSE:
	/* flush audio FIFO */
	pthread_mutex_lock( &ao->read_mutex );

	ao->fifo_read_ptr = ao->fifo_write_ptr;

	if (ao->fifo_flush == 1)
	{
	    ao->fifo_flush = 0;
	    ao->fifo_delay = 0;
	}
	pthread_mutex_unlock( &ao->read_mutex );

	break;

    case AO_CTRL_PLAY_RESUME:
	break;
    }
    return 0;
}

xine_audio_port_t *init_audio_out_plugin( xine_t *xine, xine_arts_audio *audio,
					  void **ao_driver )
{
    fifo_driver_t *ao = (fifo_driver_t *)malloc( sizeof(fifo_driver_t) );

    ao->audio		= audio;
    ao->fifo		= NULL;
    ao->fifo_read_ptr	= 0;
    ao->fifo_write_ptr	= 0;
    ao->fifo_flush	= 2;
    ao->fifo_delay	= 0;
    ao->capabilities	= (AO_CAP_MODE_MONO | AO_CAP_MODE_STEREO);

    ao->ao_driver.get_capabilities	= ao_fifo_get_capabilities;
    ao->ao_driver.get_property		= ao_fifo_get_property;
    ao->ao_driver.set_property		= ao_fifo_set_property;
    ao->ao_driver.open			= ao_fifo_open;
    ao->ao_driver.num_channels		= ao_fifo_num_channels;
    ao->ao_driver.bytes_per_frame	= ao_fifo_bytes_per_frame;
    ao->ao_driver.delay			= ao_fifo_delay;
    ao->ao_driver.write			= ao_fifo_write;
    ao->ao_driver.close			= ao_fifo_close;
    ao->ao_driver.exit			= ao_fifo_exit;
    ao->ao_driver.get_gap_tolerance	= ao_fifo_get_gap_tolerance;
    ao->ao_driver.control		= ao_fifo_control;

    pthread_cond_init( &ao->cond, NULL );
    pthread_mutex_init( &ao->read_mutex, NULL );
    pthread_mutex_init( &ao->write_mutex, NULL );

    *ao_driver		= (void *)ao;

    return _x_ao_new_port( xine, (ao_driver_t *)ao, 0 );
}

unsigned long ao_fifo_read( void *ao_driver, unsigned char **buffer,
			    unsigned long samples )
{
    fifo_driver_t *ao = (fifo_driver_t *)ao_driver;
    int bytes_in_buffer, bytes_to_read;

    /* lock read buffer */
    pthread_mutex_lock( &ao->read_mutex );

    bytes_in_buffer = ao_fifo_bytes_in_buffer( ao );
    bytes_to_read   = (samples * ao->bytes_per_frame);

    if (ao->fifo_flush || bytes_in_buffer == 0)
    {
	/* unlock read buffer */
	pthread_mutex_unlock( &ao->read_mutex );

	/* signal blocked writes */
	pthread_mutex_lock( &ao->write_mutex );
	pthread_cond_signal( &ao->cond );
	pthread_mutex_unlock( &ao->write_mutex );

	/* audio FIFO empty or disabled, return */
	return 0;
    }

    if (bytes_to_read > bytes_in_buffer)
    {
	fprintf( stderr, "fifo_audio_out: audio buffer underflow!\n" );
	bytes_to_read = bytes_in_buffer - (bytes_in_buffer % ao->bytes_per_frame);
    }
    if ((ao->fifo_read_ptr + bytes_to_read) > ao->fifo_size)
    {
	/* copy samples from front to end of buffer */
	memcpy( &ao->fifo[ao->fifo_size], ao->fifo,
		((ao->fifo_read_ptr + bytes_to_read) - ao->fifo_size) );
    }

    /* return pointer to audio samples */
    *buffer = &ao->fifo[ao->fifo_read_ptr];

    return bytes_to_read;
}

void ao_fifo_flush( void *ao_driver, unsigned long samples )
{
    fifo_driver_t *ao = (fifo_driver_t *)ao_driver;
    int bytes_in_buffer, bytes_to_flush;

    /* flush audio data */
    bytes_in_buffer = ao_fifo_bytes_in_buffer( ao );
    bytes_to_flush  = (samples * ao->bytes_per_frame);

    if (bytes_to_flush <= bytes_in_buffer)
    {
	int new_read_ptr = (ao->fifo_read_ptr + bytes_to_flush);

	if (new_read_ptr >= ao->fifo_size)
	{
	    new_read_ptr -= ao->fifo_size;
	}
	ao->fifo_read_ptr = new_read_ptr;
    }

    /* unlock read buffer */
    pthread_mutex_unlock( &ao->read_mutex );

    /* signal blocked writes */
    pthread_mutex_lock( &ao->write_mutex );
    pthread_cond_signal( &ao->cond );
    pthread_mutex_unlock( &ao->write_mutex );
}

void ao_fifo_clear( void *ao_driver, int clear )
{
    fifo_driver_t *ao = (fifo_driver_t *)ao_driver;

    pthread_mutex_lock( &ao->write_mutex );

    /* enable/disable audio driver */
    ao->fifo_flush = clear;
    ao->fifo_delay = 0;

    if (clear)
    {
	/* signal blocked writes */
	pthread_cond_signal( &ao->cond );
    }
    pthread_mutex_unlock( &ao->write_mutex );
}
