/*
   This file is part of KDE/aRts (Noatun) - xine integration
   Copyright (C) 2002-2003 Ewald Snel <ewald@rambo.its.tudelft.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
*/

#ifndef __AUDIO_FIFO_OUT_H
#define __AUDIO_FIFO_OUT_H

#include <xine.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int32_t	sample_rate;
    uint32_t	num_channels;
    uint32_t	bits_per_sample;
} xine_arts_audio;


xine_audio_port_t *init_audio_out_plugin( xine_t *xine, xine_arts_audio *audio,
					  void **ao_driver );

int ao_fifo_arts_delay();

unsigned long ao_fifo_read( void *ao_driver, unsigned char **buffer,
			    unsigned long samples );

void ao_fifo_flush( void *ao_driver, unsigned long samples );

void ao_fifo_clear( void *ao_driver, int clear );

#ifdef __cplusplus
}
#endif

#endif
